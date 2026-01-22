/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 12:58:19 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/22 15:55:32 by mait-all         ###   ########.fr       */
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

bool	Server::isCgiPipeFd(int fd)
{
	return (_cgiPipeToClient.find(fd) != _cgiPipeToClient.end());
}

void	Server::handleCgiError(int clientFd, int pipeFd)
{
	Client& client = _clients[clientFd];
	
	if (client.getCgiPid() > 0)
	{
		kill(client.getCgiPid(), SIGKILL);
		waitpid(client.getCgiPid(), NULL, 0);
	}
	_epoll.delFd(pipeFd);
	close(pipeFd);
    Request& req = _clientRequests[clientFd];
    std::string errorResponse = "HTTP/1.1 500 Internal Server Error\r\n"
                               "Content-Type: text/plain\r\n"
                               "Content-Length: 21\r\n\r\n"
                               "500 Internal Server Error\n";
    req.setCgiResponse(errorResponse);
    
    // Switch to write mode
    _epoll.modFd(clientFd, EPOLLOUT);
    
    // Cleanup
    _cgiPipeToClient.erase(pipeFd);
    client.setCgiRunning(false);
}

void	Server::handleCgiOutput(int pipeFd, struct epoll_event& event)
{
	if (_cgiPipeToClient.find(pipeFd) == _cgiPipeToClient.end())
		return ;
	
	int clientFd = _cgiPipeToClient[pipeFd];
	Client&	client = _clients[clientFd];

	if (event.events & (EPOLLIN | EPOLLHUP | EPOLLERR) )
	{
		char buffer[MAX_BUFFER_SIZE];
		
		ssize_t bytesRead = _cgiHandler.readChunk(pipeFd, buffer, sizeof(buffer));
		if (bytesRead > 0)
		{
			client.appendCgiOutput(std::string(buffer, bytesRead));
            std::cout << "📥 Read " << bytesRead << " bytes from CGI for client " 
                      << clientFd << std::endl;
		}
		else if (bytesRead == 0)
		{
            std::cout << "✅ CGI finished for client " << clientFd << std::endl;
			_epoll.delFd(pipeFd);
			close(pipeFd);

			int exitStatus;
			_cgiHandler.checkCgiStatus(client.getCgiPid(), exitStatus);
			std::cout << "exit status is : => " << exitStatus << std::endl;
			if (exitStatus != 0)
			{
				std::string	errorResponse = "HTTP/1.1 500 Internal Server Error\r\n"
                               				"Content-Type: text/html\r\n"
                               				"Content-Length: 26\r\n\r\n"
                               				"500 Internal Server Error\n";
				Request&	req = _clientRequests[clientFd];
				req.setCgiResponse(errorResponse);
				client.setCgiRunning(false);
				_epoll.modFd(clientFd, EPOLLOUT);
				_cgiPipeToClient.erase(pipeFd);
				return ;
			}
			
			std::string cgiOutput = client.getCgiOutput();
			std::string response = buildCgiResponse(cgiOutput);
			
			Request& req = _clientRequests[clientFd];
			req.setCgiResponse(response);

			client.setCgiRunning(false);
			_epoll.modFd(clientFd, EPOLLOUT);
			_cgiPipeToClient.erase(pipeFd);
		}
		else
		{
            std::cerr << "❌ Error reading from CGI pipe\n";
            handleCgiError(clientFd, pipeFd);
		}
	}
}

void	Server::startCgiForClient(int clientFd, const Request& req)
{
	Client&	client = _clients[clientFd];

	pid_t	pid;
	int pipeFd = _cgiHandler.startCgiScript(req, pid);

	std::cout << "pipeFd for clientFd: " << clientFd << " is : " << pipeFd << std::endl;
	
	if (pipeFd < 0)
	{
		std::cout << "error occured during executing cgi script!" << std::endl;
		_exit(190);
	} 
	
	client.setCgiPipeEnd(pipeFd);
	client.setCgiPid(pid);
	client.setCgiRunning(true);

	_epoll.addFd(pipeFd, EPOLLIN);

	_cgiPipeToClient[pipeFd] = clientFd;


    std::cout << "✅ CGI started for client " << clientFd 
              << " (pipe fd: " << pipeFd << ", pid: " << pid << ")\n";
}

std::string Server::ft_trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::map<std::string, std::string> Server::parseCgiHeaders(const std::string& rawHeaders) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(rawHeaders);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove \r if present (handles both \n and \r\n)
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        
        // Skip empty lines
        if (line.empty())
            continue;
        
        // Find the colon separator
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            // Malformed header - skip it
            continue;
        }
        
        // Extract header name and value
        std::string header_name = ft_trim(line.substr(0, colon_pos));
        std::string header_value = ft_trim(line.substr(colon_pos + 1));
        
        // Convert header name to lowercase for case-insensitive comparison
        // (HTTP headers are case-insensitive)
        for (size_t i = 0; i < header_name.length(); i++) {
            header_name[i] = std::tolower(header_name[i]);
        }
        
        // Store the header
        if (!header_name.empty()) {
            headers[header_name] = header_value;
        }
    }
    
    return headers;
}


std::string Server::buildCgiResponse(const std::string& cgiOutput)
{
    std::string response;
    
	size_t	headerEnd = cgiOutput.find("\r\n\r\n");

	if (headerEnd == std::string::npos)
		headerEnd = cgiOutput.find("\n\n");

	// std::string	cgiHeaders = cgiOutput.substr(0, headerEnd);
	// std::string cgiBody = cgiOutput.substr(headerEnd + 4);
	std::stringstream ss;
	std::map<std::string, std::string>	headers;
	std::string	body;
	if (headerEnd != std::string::npos)
	{
		std::string headersStr = cgiOutput.substr(0, headerEnd);
		headers = parseCgiHeaders(headersStr);
		body = cgiOutput.substr(headerEnd + (cgiOutput[headerEnd] == '\r' ? 4 : 2));
	}
	else
		body = cgiOutput;
	if (headers.find("content-type") == headers.end())
	{
		headers["content-type"] = "text/plain";
	}

	response = "HTTP/1.1 200 OK\r\n";
	for(std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); ++it)
	{
		response += it->first + ": " + it->second + "\r\n";
	}
	ss << body.size();
	if (headers.find("content-length") == headers.end())
	{
		response += "Content-Length: " + ss.str() + "\r\n";
		response += "\r\n";
	}
	response += body;

	return (response);
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
