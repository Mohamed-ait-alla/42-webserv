/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 15:59:16 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/14 18:53:26 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
#define WEBSERV_HPP
#include <algorithm>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <climits>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <dirent.h>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <vector>
#include "Listener.hpp"
#include "Epoll.hpp"
#include "Client.hpp"
#define PORT 8080
#define IP INADDR_ANY
#define IPv4 AF_INET
// #define BACK_LOG 4096
#define MAX_BUFFER_SIZE 4096
#define MAX_EVENTS 1024

// ******************************************************************************
// //
//                                  Webserv Class //
// ******************************************************************************
// //

class Webserv {

  public:
    // * ENUM
    enum METHOD {
      GET,
      POST,
      DELETE,
      ELSE,
    };

    enum STATUS_CODE {
      // * 2xx Success
      OK = 200,
      CREATED = 201,
      NO_CONTENT = 204,

      // * 3xx Redirection
      MOVED_PERMANENTLY = 301,
      FOUND = 302,

      // * 4xx Client Error
      BAD_REQUEST = 400,
      FORBIDDEN = 403,
      NOT_FOUND = 404,
      METHOD_NOT_ALLOWED = 405,
      PAYLOAD_TOO_LARGE = 413,

      // * 5xx Server Error
      INTERNAL_SERVER_ERROR = 500,
      BAD_GATEWAY = 502,
      GATEWAY_TIMEOUT = 504
    };
    // member functions
    void throwError(std::string func);
};

// ****************************************************************************** //
//                                  Server Class                                  //
// ****************************************************************************** //

class Request;

class Server : public Webserv {
	private:
		Listener			_listener;
		Epoll				_epoll;
		

	public:
		std::map<int, Client> clients;

		// Server();
		// Server(std::string &host, int port);
		~Server();

		bool	isCompleteRequest(const std::string &request);
		size_t	getContentLength(const std::string &request);
		void	setUpNewConnection(int serverFd);
		bool	recvRequest(int notifiedFd);
		bool	sendResponse(int notifiedFd, Request &request);
		void	processClientEvent(int fd, struct epoll_event &ev, Request &req);
		void	processServerEvent(int fd);
		void	run(Request &req);
};

// ******************************************************************************
// //
//                               ConfigFile Class //
// ******************************************************************************
// //

typedef struct location {
    std::string path;
    std::vector<std::string> allow_methods;
    bool autoindex;
    std::string root;
    std::string return_to;
    std::string index;
} location;

class ConfigFile {
  public:
    std::vector<int> listen;
    std::string server_name;
    std::string host;
    std::string root;
    int client_max_body_size;
    std::string index;
    std::map<int, std::string> error_page;
    std::vector<location> locations;
    std::vector<std::string> cgi_path;
    std::vector<std::string> cgi_ext;

    void init_the_header_conf_default();
    void parse_config_file(char *av);
};

// ******************************************************************************
// //
//                                 Request Class //
// ******************************************************************************
// //

class Request : public Webserv {

  private:
    std::map<std::string, std::string> request;
    std::map<std::string, std::string> session;

  public:
    METHOD method;
    std::string path;
    std::string httpV;
    ConfigFile config;

    // * Default Contructor
    Request();

    void setRequest(const std::string &req);
    const std::map<std::string, std::string> &getRequest() const;

    void setSession(const std::string session_id, const std::string value);
    const std::map<std::string, std::string> &getSession() const;
};

// ******************************************************************************
// //
//                                 Response Class //
// ******************************************************************************
// //

class Response : public Webserv {
  private:
    std::map<std::string, std::string> mimeTypes;
    STATUS_CODE status_code;
    std::string statusLine;
    std::string contentType;
    std::string contentLength;
    std::string headers;
    size_t indexLocation;
    bool isRedirection;
    int bodyFd;

  public:
    // * Default Constructor
    Response();

    // * Getters & Setters
    void setMimeTypes();
    const std::map<std::string, std::string> &getMimeTypes() const;

    void setStatusCode(STATUS_CODE value);
    STATUS_CODE getStatusCode() const;

    void setStatusLine(const std::string httpV,
                       const std::string &statusCodeDescription);
    std::string getStatusLine() const;

    std::string getServerName(const Request &req) const;

    void setContentType(const std::string &path);
    std::string getContentType() const;

    void setContentLength(const size_t &bodyLength);
    std::string getContentLength() const;

    std::string getHeaders() const;
    void setHeaders(const Request &req);

    size_t getIndexLocation() const;
    void setIndexLocation(size_t &value);

    bool getIsRedirection() const;
    void setIsRedirection(bool value);

    // void setSession(std::string session_id, std::string value);

    void setBodyFd(int &fd);
    int getBodyFd() const;

    // * Methods
    void GET_METHOD(Request &req);
    void POST_METHOD(Request &req);
    void DELETE_METHOD(Request &req);
    std::string statusCodeDescription(STATUS_CODE statusCode);
    size_t countBodyLength(const std::string &path);
    void addDataToBody(const Request &req);
    std::map<std::string, std::string>
    parseFormURLEncoded(const std::string &post_body);
    bool thisLocationIsInConfigFile(Request &req, std::string &location);
    bool checkAllowMethodsOfLocation(std::vector<std::string> &allowMethods,
                                     std::string method);
    std::string generatePageOfAutoIndex(Request &req,
                                        std::string &pathOfAutoIndex);
    bool isPathStartBySlash(const std::string &path);
    bool isFile(std::string path);
    bool isUserInSession(const Request &req, std::string session_id);
    void generateResponse(Request &req);
    void methodNotAllowed(Request &req);
    void response(Request &req);
};

#endif