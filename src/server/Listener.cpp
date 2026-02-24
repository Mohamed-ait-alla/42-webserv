/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Listener.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/12 17:53:32 by mait-all          #+#    #+#             */
/*   Updated: 2026/02/04 09:28:56 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../include/server/Listener.hpp"

Listener::Listener() : _host("127.0.0.1"), _sockfd(-1), _port(-1)
{
	std::memset(&_serverAddr, 0, sizeof(_serverAddr));
}

Listener::~Listener()
{
	for (size_t i = 0; i < _serverSockets.size(); i++)
	{
		close(_serverSockets[i]);
	}
}

void	Listener::initServerAddress()
{
	_serverAddr.sin_family = IPv4;
	_serverAddr.sin_addr.s_addr = inet_addr(_host.c_str());
	_serverAddr.sin_port = htons(_port);
	std::memset(_serverAddr.sin_zero, 0, sizeof(_serverAddr.sin_zero));
}

void	Listener::createServerSocket()
{
	int	opt = 1;

	_sockfd = socket(IPv4, SOCK_STREAM, 0);
	if (_sockfd < 0)
		throwError("socket()");
		
	if (setsockopt(_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
		throwError("setsockopt()");
	
	setNonBlocking(_sockfd);
}

void	Listener::bindServerSocket()
{
	if (bind(_sockfd, (struct sockaddr *)&_serverAddr, sizeof(_serverAddr)) < 0)
		throwError("bind()");
}

void	Listener::startListening()
{
	if (listen(_sockfd, BACK_LOG) < 0)
		throwError("listen()");
	
	_serverSockets.push_back(_sockfd);
}

bool	Listener::isListeningSocket(int fd)
{
	if (std::find(_serverSockets.begin(), _serverSockets.end(), fd) != _serverSockets.end())
		return (true);
	return (false);
}

void	Listener::setHost(std::string& host)
{
	_host = host;
}

void	Listener::setSockFd(int fd)
{
	_sockfd = fd;
}

void	Listener::setPort(int port)
{
	_port = port;
}

std::string	Listener::getHost() const
{
	return (_host);
}

int	Listener::getSockFd() const
{
	return (_sockfd);
}

int	Listener::getPort() const
{
	return (_port);
}

void	Listener::throwError(std::string func)
{
  throw std::runtime_error(func + "failed: " + strerror(errno));
}