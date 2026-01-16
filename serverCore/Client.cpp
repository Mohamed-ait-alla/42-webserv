/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 13:13:27 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/16 16:23:52 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"


Client::Client()
	: _clientFd(-1)
{	
}

Client::Client(int clientFd)
	: _bytesReceived(0),
	  _contentLength(0),
	  _isPostRequest(false),
	  _requestComplete(false),
	  _isHeaderSent(false),
	  _bodyFd(-1)
{
	_clientFd = clientFd;
}

// member functions

// setters
void	Client::setIsPostRequest(bool val)
{
	_isPostRequest = val;
}

void	Client::setRequestComplete(bool val)
{
	_requestComplete = val;
}

void	Client::setContentLength(size_t length)
{
	_contentLength = length;
}

void	Client::setHeaderSent(bool val)
{
	_isHeaderSent = val;
}

void	Client::setBodyFd(int fd)
{
	_bodyFd = fd;
}

// getters

const std::string&	Client::getRequest() const
{
	return (_request);
}

bool	Client::isHeaderSent() const
{
	return (_isHeaderSent);
}

int	Client::getBodyFd() const
{
	return (_bodyFd);
}

void	Client::appendRequest(const std::string& data, size_t length)
{

	_request.append(data.c_str(), length);
	_bytesReceived += length;
}

size_t	Client::findHeaderEnd() const
{
	return (_request.find("\r\n\r\n"));
}

bool	Client::hasCompleteHeaders() const
{
	return (_request.find("\r\n\r\n") != std::string::npos);
}

bool	Client::hasCompleteBody() const
{
	size_t	headerEnd = findHeaderEnd();
	if (headerEnd == std::string::npos)
		return (false);

	if (!_isPostRequest)
		return (true);

	size_t bodyStart = headerEnd + 4;
	size_t currentBodySize = _request.length() - bodyStart;
	return (currentBodySize >= _contentLength);
}


