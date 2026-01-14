/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 13:05:03 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/14 18:32:25 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

// Default constructor
// Server::Server()
// {
// 	_sockfd = -1;
// }

// Server::Server(std::string &host, int port)
// 	: _sockfd(-1), _epollfd(-1), _port(port), _host(host)
// {
// 	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
// }

Server::~Server()
{
	for (size_t i = 0; i < _listener._serverSockets.size(); i++)
	{
		close(_listener._serverSockets[i]);
	}

	if (_epoll.getEpollFd() >= 0)
		close(_epoll.getEpollFd());
}


bool Server::isCompleteRequest(const std::string &request)
{
	if (request.find("Content-Length:") != std::string::npos)
		return (true);
	return (false);
}

size_t Server::getContentLength(const std::string &request)
{
	size_t pos = request.find("Content-Length:");
	if (pos == std::string::npos)
		return (0);
	size_t start = pos + 15;
	size_t end = request.find("\r\n", start);
	if (end == std::string::npos)
		return (0);

	std::string lengthStr = request.substr(start, end - start);
	return (std::atoll(lengthStr.c_str()));
}

void Server::setUpNewConnection(int serverFd)
{
	int clientFd;

	clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0)
		return;

	_listener.setNonBlocking(clientFd);
	_epoll.addFd(clientFd, EPOLLIN);

	clients[clientFd] = Client(clientFd);

	std::cout << "\n✅ New connection accpeted (servfd: " << serverFd << " clientfd: " << clientFd << ")\n"
			  << std::endl;
}

bool Server::recvRequest(int notifiedFd)
{
	char buffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead;
	bytesRead = recv(notifiedFd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (bytesRead <= 0)
	{
		close(notifiedFd);
		clients.erase(notifiedFd);
		return (false);
	}
	
	Client&	client = clients[notifiedFd];
	
	client.appendRequest(buffer, bytesRead);
	if (!client.hasCompleteHeaders())
		return (false);
	if (isCompleteRequest(client.getRequest()))
	{
		size_t contentLength = getContentLength(client.getRequest());
		client.setContentLength(contentLength);
		if (contentLength > 0)
			client.setIsPostRequest(true);
	}

	if (!client.hasCompleteBody())
		return (false);

	std::cout << "=== Request received ===\n";
	std::cout << client.getRequest() << std::endl;

	_epoll.modFd(notifiedFd, EPOLLIN | EPOLLOUT);

	return (true);
}

bool Server::sendResponse(int notifiedFd, Request &request)
{
	Client&	client = clients[notifiedFd];

	if (!client.isHeaderSent())
	{
		Response res;

		res.response(request);
		std::string responseHeaders = res.getHeaders();
		size_t headersLength = responseHeaders.length();
		ssize_t bytesSent;

		bytesSent = send(notifiedFd, responseHeaders.c_str(), headersLength, 0);
		if (bytesSent < 0)
			throwError("send() when sending header part");
		client.setHeaderSent(true);
		client.setBodyFd(res.getBodyFd());
	}
	if (client.isHeaderSent())
	{
		char buffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead;
		ssize_t bytesSent;
		bytesRead = read(client.getBodyFd(), buffer, sizeof(buffer));
		if (bytesRead <= 0)
		{
			clients.erase(notifiedFd);
			_epoll.delFd(notifiedFd);
			close(notifiedFd);
			close(client.getBodyFd());
			return (true);
		}
		bytesSent = send(notifiedFd, buffer, bytesRead, 0);
		if (bytesSent < 0)
		{
			close(notifiedFd);
			clients.erase(notifiedFd);
			return (false);
		}
		std::memset(buffer, '\0', sizeof(buffer));
	}
	return (false);
}

void Server::processServerEvent(int fd)
{
	setUpNewConnection(fd);
}

void Server::processClientEvent(int fd, struct epoll_event &event, Request &req)
{
	if (event.events & EPOLLIN) // Read event => Request received
	{
		if (!recvRequest(fd))
			return;
		req.setRequest(clients[fd].getRequest());
	}
	else if (event.events & EPOLLOUT) // Write event => Send response
	{
		if (!sendResponse(fd, req))
			return;
	}
	else // Error event => EPOLLERR
	{
		_epoll.delFd(fd);
		close(fd);
		return;
	}
}


void Server::run(Request &req)
{
	int n_fds;

	for (size_t i = 0; i < req.config.listen.size(); i++)
	{
		_listener.setHost(req.config.host);
		_listener.setPort(req.config.listen[i]);

		// init server address
		_listener.initServerAddress();

		// socket creation
		_listener.createServerSocket();

		// socket identification
		_listener.bindServerSocket();

		// socket listening
		_listener.startListening();

		// _listener._serverSockets.push_back(_listener.getSockFd());
		std::cout << "🚀 Server running on " << _listener.getHost() << ":" << _listener.getPort() << std::endl;
	}

	// epoll's instance creation
	_epoll.createEpollInstance();

	// setup server socket to accept new connections
	for(size_t i = 0; i < _listener._serverSockets.size(); i++)
	{
		_epoll.addFd(_listener._serverSockets[i], EPOLLIN);
	}

	// running server and waiting for connections
	struct epoll_event events[MAX_EVENTS];

	bool running = true;
	while (running)
	{
		n_fds = _epoll.wait(events, MAX_EVENTS);
		for (int i = 0; i < n_fds; i++)
		{
			int fd = events[i].data.fd;

			if (_listener.isListeningSocket(fd))
				processServerEvent(fd);
			else
				processClientEvent(fd, events[i], req);
		}
	}
}
