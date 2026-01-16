/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.cpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 05:51:49 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/16 17:13:03 by mait-all         ###   ########.fr       */
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
	std::cout << "in connectin: " << _epollInstance.getEpollFd() << std::endl;;
	_epollInstance.addFd(clientFd, EPOLLIN);
	
	clients[clientFd] = Client(clientFd);

	std::cout << "\n✅ New connection accpeted (servfd: " << serverFd << " clientfd: " << clientFd << ")\n" << std::endl;
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


bool ConnectionManager::receiveData(int clientFd, std::map<int, Client>& clients)
{
	char buffer[MAX_BUFFER_SIZE];
	ssize_t bytesRead;

	bytesRead = recv(clientFd, buffer, MAX_BUFFER_SIZE - 1, 0);
	if (bytesRead <= 0)
	{
		close(clientFd);
		clients.erase(clientFd);
		return (false);
	}

	Client&	client = clients[clientFd];
	
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

	return (true);
}

bool ConnectionManager::sendData(int clientFd, std::map<int, Client>& clients, Request& req)
{
	Client&	client = clients[clientFd];

	if (!client.isHeaderSent())
	{
		Response res;

		res.response(req);
		std::string responseHeaders = res.getHeaders();
		size_t headersLength = responseHeaders.length();
		ssize_t bytesSent;

		bytesSent = send(clientFd, responseHeaders.c_str(), headersLength, 0);
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
			clients.erase(clientFd);
			_epollInstance.delFd(clientFd);
			close(clientFd);
			close(client.getBodyFd());
			return (true);
		}
		bytesSent = send(clientFd, buffer, bytesRead, 0);
		if (bytesSent < 0)
		{
			close(clientFd);
			clients.erase(clientFd);
			return (false);
		}
		std::memset(buffer, '\0', sizeof(buffer));
	}
	return (false);
}