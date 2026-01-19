/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/18 13:05:03 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/17 15:09:57 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

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
		if (now - it->second.getLastActivity() > CLIENT_TIMEOUT)
		{
			int fd = it->first;
			it->second.setTimedOut();
			++it;
			_connectionManager.closeConnection(fd, _clients);
		}
		else
			++it;
	}
}

bool	Server::receiveRequest(int clientFd)
{
	if (!_connectionManager.receiveData(clientFd, _clients))
		return (false);
	return (true);
}

bool	Server::sendResponse(int clientFd, Request& req)
{
	if (!_connectionManager.sendData(clientFd, _clients, req))
		return (false);
	return (true);
}

void	Server::handleError(int clientFd)
{
	std::cout << "❌ Error or closing connection for client " << clientFd << std::endl;

	_epoll.delFd(clientFd);
	
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

void Server::processClientEvent(int fd, struct epoll_event &event, Request &req)
{
	if (event.events & EPOLLIN) // Read event => Request received
	{
		if (receiveRequest(fd))
		{
			_epoll.modFd(fd, EPOLLIN | EPOLLOUT);
			req.setRequest(_clients[fd].getRequest());
		}
	}
	else if (event.events & EPOLLOUT) // Write event => Send response
	{
		if (!sendResponse(fd, req))
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

		std::cout << "🚀 Server running on " << _listener.getHost() << ":" << _listener.getPort() << std::endl;
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
