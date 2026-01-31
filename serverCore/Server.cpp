/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 12:58:19 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/31 09:48:23 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include <cstring>

// Default constructor
Server::Server()
	: _listener(),
	  _epoll(),
	  _connectionManager(_epoll)
{
}

Server::~Server()
{
	for (size_t i = 0; i < _listener._serverSockets.size(); i++)
	{
		close(_listener._serverSockets[i]);
	}
}

void	Server::checkClientTimeOut()
{
	time_t	now = time(NULL);

	for(std::map<int, Client>::iterator it = _clients.begin(); it != _clients.end(); )
	{
		Client&	client = it->second;

		if (client.isCgiRunning())
		{
			if (now - client.getCgiStartTime() > CGI_TIMEOUT)
			{
				logMessage(LOG_TIMEOUT,
							"CGI timed out for client fd=" +
							toString(client.getClientFd()) +
							"(pid=" + toString(client.getCgiPid()) + ")");

				client.setCgiTimedOut(true);
				handleCgiError(client.getClientFd(), client.getCgiPipeEnd());
			}
		}

		if (now - client.getLastActivity() > CLIENT_TIMEOUT)
		{
			int fd = it->first;
			client.setTimedOut();
			++it;
			_connectionManager.closeConnection(fd, _clients, _cgiPipeToClient, "Client timeout");
		}
		else
			++it;
	}
}

bool	Server::receiveRequest(int clientFd)
{
	if (!_connectionManager.receiveData(clientFd, _clients, _cgiPipeToClient))
		return (false);
	return (true);
}

bool	Server::sendResponse(int clientFd, Request& req)
{
	if (!_connectionManager.sendData(clientFd, _clients, _cgiPipeToClient, req))
		return (false);
	return (true);
}

void	Server::handleError(int clientFd)
{
	std::cout << "❌ Error or closing connection for client " << clientFd << std::endl;

	_epoll.delFd(clientFd, "Error OR Client closed connection");
	
	Client& client = _clients[clientFd];
	if (client.getBodyFd() >= 0)
		close(client.getBodyFd());
	close(clientFd);
	_clients.erase(clientFd);
}

void	Server::processServerEvent(int fd)
{
	_connectionManager.setUpNewConnection(fd, _clients);
}

bool	Server::isCgiPipeFd(int fd)
{
	return (_cgiPipeToClient.find(fd) != _cgiPipeToClient.end());
}

void	Server::handleCgiError(int clientFd, int pipeFd)
{
	Client& client = _clients[clientFd];
	Request& req = _clientRequests[clientFd];
	

	if (client.isCgiTimedOut())
	{
		if (client.getCgiPid() > 0)
		{
			kill(client.getCgiPid(), SIGKILL);
			waitpid(client.getCgiPid(), NULL, 0);
		}
		_epoll.delFd(pipeFd, "CGI timed out");
		close(pipeFd);
		
		std::string	body = loadErrorPage(504);
		
		std::map<std::string, std::string> headers;
		headers["Content-Type"] = "text/html";
		headers["Content-Length"] = toString(body.size());
		
		req.setCgiResponse(buildCgiResponse(504, "Gateway Timeout", headers, body));
		
		_epoll.modFd(clientFd, EPOLLOUT);
		_cgiPipeToClient.erase(pipeFd);
		client.setCgiRunning(false);
		client.setStatusCode(Webserv::GATEWAY_TIMEOUT);
	}
	else
	{
		std::string body = loadErrorPage(500);

		std::map<std::string, std::string> headers;
		headers["Content-Type"] = "text/html";
		headers["Content-Length"] = toString(body.size());

		req.setCgiResponse(buildCgiResponse(500, "Internal Server Error", headers, body));
		client.setStatusCode(Webserv::INTERNAL_SERVER_ERROR);
	}
}

void Server::handleCgiOutput(int pipeFd, struct epoll_event& event)
{
	if (_cgiPipeToClient.find(pipeFd) == _cgiPipeToClient.end())
		return;

	int clientFd = _cgiPipeToClient[pipeFd];
	Client& client = _clients[clientFd];

	if (!(event.events & (EPOLLIN | EPOLLHUP | EPOLLERR)))
		return;

	char buffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead = _cgiHandler.readChunk(pipeFd, buffer, sizeof(buffer));

	if (bytesRead > 0)
	{
		client.appendCgiOutput(std::string(buffer, bytesRead));
		return;
	}

	// CGI finished or failed
	_epoll.delFd(pipeFd, "CGI finished or failed");
	close(pipeFd);

	int exitStatus;
	_cgiHandler.checkCgiStatus(client.getCgiPid(), exitStatus);

	Request& req = _clientRequests[clientFd];

	if (bytesRead < 0 || exitStatus != 0)
		handleCgiError(clientFd, pipeFd);
	else
	{
		CgiResult cgi = parseCgiOutput(client.getCgiOutput());

		req.setCgiResponse(buildCgiResponse(200, "OK", cgi.headers, cgi.body));
		client.setStatusCode(Webserv::OK);
	}

	client.setCgiRunning(false);
	_epoll.modFd(clientFd, EPOLLOUT);
	_cgiPipeToClient.erase(pipeFd);
}


void	Server::startCgiForClient(int clientFd, const Request& req)
{
	Client&	client = _clients[clientFd];

	pid_t	pid;
	client.setCgiStartTime(time(NULL));
	int pipeFd = _cgiHandler.startCgiScript(req, pid);
	
	if (pipeFd < 0)
	{
		handleCgiError(clientFd, pipeFd);
		_epoll.modFd(clientFd, EPOLLOUT);
		return ;
	} 
	
	client.setCgiPipeEnd(pipeFd);
	client.setCgiPid(pid);
	client.setCgiRunning(true);

	_epoll.addFd(pipeFd, EPOLLIN);

	_cgiPipeToClient[pipeFd] = clientFd;

	logMessage(LOG_CGI,
				"    client fd=" + toString(clientFd) +
				" started (pid=" + toString(pid) + ", pipeFd=" +
				toString(pipeFd) + ")");
}

void	Server::processClientEvent(int fd, struct epoll_event &event, Request &req)
{
	if (isCgiPipeFd(fd))
	{
		handleCgiOutput(fd, event);
		return ;
	}
	if (event.events & EPOLLIN) // Read event => Request received
	{
		if (receiveRequest(fd))
		{
			req.setRequest(_clients[fd].getRequest());
			_clientRequests[fd] = req;
			logMessage(LOG_REQ,
						"    fd=" + toString(fd) + " " +
						req.getMethodByName(req.method) + " " +
						req.path + " HTTP/1.1");

			if (req.getIsCGI())
				startCgiForClient(fd, req);
			else
				_epoll.modFd(fd, EPOLLOUT);
		}
	}
	else if (event.events & EPOLLOUT) // Write event => Send response
	{
		Request& request = _clientRequests[fd];
		if (!sendResponse(fd, request))
			return;
	}
	else // Error event => EPOLLERR
	{
		handleError(fd);
		return;
	}
}

void	Server::run(Request &req)
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
		
		logMessage(LOG_INFO,
					"   🚀 Server running on " +
					_listener.getHost() + ":" +
					toString(_listener.getPort()));
	}

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

		checkClientTimeOut();
	}
}
