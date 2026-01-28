/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 05:51:49 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/28 17:03:01 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/ConnectionManager.hpp"

ConnectionManager::ConnectionManager(Epoll& epoll)
	: _epollInstance(epoll)
{
}

void	ConnectionManager::setUpNewConnection(int serverFd, std::map<int, Client>& clients)
{
	int	clientFd;

	clientFd = accept(serverFd, NULL, NULL);
	if (clientFd < 0)
		return ;

	setNonBlocking(clientFd);

	logMessage(LOG_CONN,
		"   Accepted client fd=" + toString(clientFd) +
		" (server fd=" + toString(serverFd) + ")");

	_epollInstance.addFd(clientFd, EPOLLIN);
	clients.insert(std::make_pair(clientFd, Client(clientFd)));
}

bool ConnectionManager::isCompleteRequest(const std::string &request)
{
	if (request.find("Content-Length:") != std::string::npos)
		return (true);
	return (false);
}

size_t ConnectionManager::getContentLength(const std::string &request)
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


bool ConnectionManager::receiveData(int clientFd, std::map<int, Client>& clients, std::map<int, int>& cgiPipeToClient)
{
	char buffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead;

	bytesRead = recv(clientFd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (bytesRead <= 0)
	{
		closeConnection(clientFd, clients, cgiPipeToClient, "Client closed connection");
		return (false);
	}

	Client&	client = clients[clientFd];
	buffer[bytesRead] = '\0';
	client.appendRequest(buffer, bytesRead);
	client.updateLastActivity();
	if (!client.hasCompleteHeaders())
		return (false);
	if (isCompleteRequest(client.getRequest()))
	{
		size_t contentLength = getContentLength(client.getRequest());
		client.setContentLength(contentLength);
		client.setIsPostRequest(contentLength > 0);
	}

	if (!client.hasCompleteBody())
		return (false);

	// std::cout << "=== Request received ===\n";
	// std::cout << client.getRequest() << std::endl;

	return (true);
}

bool ConnectionManager::sendData(int clientFd, std::map<int, Client>& clients, std::map<int, int>& cgiPipeToClient, Request& req)
{
	if (req.getIsCGI())
	{
		Client&	client = clients[clientFd];

		ssize_t bytesSent = send(clientFd, req.getCgiResponse().c_str(), req.getCgiResponse().size(), 0);
		if (bytesSent <= 0)
		{
			closeConnection(clientFd, clients, cgiPipeToClient, "failed to send cgi response");
			return (true);
		}
		logMessage(LOG_RESP,
			"   fd=" + toString(clientFd) + " " +
			toString(client.getStatusCode()) + " " +
			statusCodeToString(client.getStatusCode()));

		closeConnection(clientFd, clients, cgiPipeToClient, "cgi response sent");
		return (true);
	}
	else
	{
		Client&	client = clients[clientFd];
		Response res;
	
		if (!client.isHeaderSent())
		{
				res.response(req);
				std::string responseHeaders = res.getHeaders();
				// std::cout << "--->status code: " << res.getStatusCode() << std::endl;
				size_t headersLength = responseHeaders.length();
				ssize_t bytesSent;
		
				bytesSent = send(clientFd, responseHeaders.c_str(), headersLength, 0);
				if (bytesSent <= 0)
				{
					close(clientFd);
					clients.erase(clientFd);
					return (false);
				}
				client.setHeaderSent(true);
				client.setBodyFd(res.getBodyFd());
				client.setStatusCode(res.getStatusCode());
				client.updateLastActivity();
		}
	
		char buffer[MAX_BUFFER_SIZE];
		ssize_t bytesRead;
		ssize_t bytesSent;
	
		bytesRead = read(client.getBodyFd(), buffer, sizeof(buffer));
		if (bytesRead <= 0)
		{
			close(client.getBodyFd());
			logMessage(LOG_RESP,
				"   fd=" + toString(clientFd) + " " +
				toString(client.getStatusCode()) + " " +
				statusCodeToString(client.getStatusCode()));

			closeConnection(clientFd, clients, cgiPipeToClient, "response sent");
			return (true);
		}
	
		bytesSent = send(clientFd, buffer, bytesRead, 0);
		if (bytesSent <= 0)
		{
			close(clientFd);
			clients.erase(clientFd);
			return (false);
		}
	
		client.updateLastActivity();
		return (false);
	}
}

void	ConnectionManager::closeConnection(int clientFd,
											std::map<int, Client>& clients,
											std::map<int, int>& cgiPipeToClient,
											const std::string&	reason)
{
	std::map<int, Client>::iterator it = clients.find(clientFd);
	if (it == clients.end())
		return;

	_epollInstance.delFd(clientFd, reason);
	close(clientFd);

	if (it->second.getIsTimedOut())
		logMessage(LOG_TIMEOUT, "client fd=" + toString(clientFd) + " timed out");

	if (it->second.isCgiRunning())
	{
		pid_t	pid = it->second.getCgiPid();
		int		pipeFd = it->second.getCgiPipeEnd();
		if (pid > 0)
		{
			kill(pid, SIGKILL);
			waitpid(pid, NULL, 0);
		}
		_epollInstance.delFd(pipeFd, reason);
		close(pipeFd);
		cgiPipeToClient.erase(pipeFd);
	}

	clients.erase(it);

	logMessage(LOG_DEBUG, "  client fd=" + toString(clientFd) + " removed from clients map");
}