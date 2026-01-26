/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 13:13:27 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/23 16:36:26 by mait-all         ###   ########.fr       */
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
	  _lastActivity(time(NULL)),
	  _isPostRequest(false),
	  _requestComplete(false),
	  _isHeaderSent(false),
	  _isTimedOut(false),
	  _clientFd(clientFd),
	  _bodyFd(-1),
	  _cgiPipeFd(-1),
	  _cgiPid(-1),
	  _isCgiRunning(false),
	  _isCgiTimedOut(false),
	  _cgiStartTime(-1)
{
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

void	Client::updateLastActivity()
{
	_lastActivity = time(NULL);
}

void	Client::setTimedOut()
{
	_isTimedOut = true;
}

void	Client::setCgiPipeEnd(int fd)
{
	_cgiPipeFd = fd;
}

void	Client::setCgiPid(pid_t pid)
{
	_cgiPid = pid;
}

void	Client::setCgiRunning(bool val)
{
	_isCgiRunning = val;
}

void	Client::setCgiStartTime(time_t val)
{
	_cgiStartTime = val;
}

void	Client::setCgiTimedOut(bool val)
{
	_isCgiTimedOut = val;
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

time_t	Client::getLastActivity() const
{
	return (_lastActivity);
}

int		Client::getClientFd() const
{
	return (_clientFd);
}

bool	Client::getIsTimedOut() const
{
	return (_isTimedOut);
}

int	Client::getCgiPipeEnd() const
{
	return (_cgiPipeFd);
}

pid_t	Client::getCgiPid() const
{
	return (_cgiPid);
}

bool	Client::isCgiRunning() const
{
	return (_isCgiRunning);
}

const std::string& Client::getCgiOutput() const
{
	return (_cgiOutput);
}

time_t	Client::getCgiStartTime() const
{
	return (_cgiStartTime);
}

bool	Client::isCgiTimedOut() const
{
	return (_isCgiTimedOut);
}



void	Client::appendCgiOutput(const std::string& data)
{
	_cgiOutput += data;
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


