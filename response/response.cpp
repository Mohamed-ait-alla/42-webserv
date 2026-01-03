/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/19 10:45:08 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/03 11:57:11 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

// * initialize server name
const std::string Response::serverName = "Server: webserv/1.0\r\n";

// * Default Constructor
Response::Response() { this->setMimeTypes(); }

// * Getters & Setters
// * status code
void Response::setStatusCode(STATUS_CODE value) { this->status_code = value; }

Webserv::STATUS_CODE Response::getStatusCode() const {
  return this->status_code;
}

void Response::setStatusLine(const std::string httpV,
                             const std::string &statusCodeDescription) {
  this->statusLine = httpV + " " + statusCodeDescription + "\r\n";
}
std::string Response::getStatusLine() const { return this->statusLine; }

// * content type
void Response::setContentType(const std::string &path) {
  size_t pos = path.rfind(".");

  if (pos == std::string::npos) {
    this->contentType =
        "Content-Type: application/octet-stream\r\n"; // * we dont know what
    // * is the type of
    // * content
    return;
  }

  std::string extention = path.substr(pos);

  std::map<std::string, std::string>::const_iterator it =
      this->getMimeTypes().find(extention);

  if (it == this->getMimeTypes().end()) {
    this->contentType =
        "Content-Type: application/octet-stream\r\n"; // * we dont know what
    // * is the type of
    // * content
    return;
  }

  this->contentType = "Content-Type: " + it->second + "\r\n";
}

std::string Response::getContentType() const { return this->contentType; }

// * content length
void Response::setContentLength(const size_t &bodyLength) {
  std::stringstream ss;
  ss << bodyLength;
  this->contentLength = "Content-Length: " + ss.str() + "\r\n";
};

std::string Response::getContentLength() const { return this->contentLength; }

// * headers
std::string Response::getHeaders() const { return this->headers; }

void Response::setHeaders() {
  this->headers = this->getStatusLine() + this->serverName +
                  this->getContentType() + getContentLength() + "\r\n";
}

void Response::setBodyFd(int &fd) { this->bodyFd = fd; }
int Response::getBodyFd() const { return this->bodyFd; }

// ! Order of response
// HTTP/1.1 200 OK
// Server: webserv/1.0
// Content-Type: text/html
// Content-Length: 128
// Connection: close

// <body>

// * GET METHOD
void Response::GET_METHOD(const Request &req) {
  std::string fullPath("pages");

  if (req.path == "/") {
    fullPath += req.path + "index.html";
  } else {
    fullPath += req.path;
  }

  // * check the file permissions and if the file exist
  if (access(fullPath.c_str(), F_OK) == -1) {
    this->setStatusCode(this->NOT_FOUND);
    fullPath = "pages/errors/404.html";
  } else if (access(fullPath.c_str(), R_OK) == -1) {
    this->setStatusCode(this->FORBIDDEN);
    fullPath = "pages/errors/403.html";
  } else {
    this->setStatusCode(this->OK);
  }

  // * Generate response
  this->generateResponse(req, fullPath);
}

// * POST METHOD
void Response::POST_METHOD(const Request &req) {
  std::string pathOfDataForm = "pages/post-request-data.html";

  // * check the file permissions and if the file exist
  if (access(pathOfDataForm.c_str(), F_OK) == -1) {
    this->setStatusCode(this->NOT_FOUND);
    pathOfDataForm = "pages/errors/404.html";
  } else if (access(pathOfDataForm.c_str(), R_OK) == -1) {
    this->setStatusCode(this->FORBIDDEN);
    pathOfDataForm = "pages/errors/403.html";
  } else {
    this->setStatusCode(this->OK);
  }

  // * Generate response
  this->generateResponse(req, pathOfDataForm);
}

// * DELETE METHOD
void Response::DELETE_METHOD(const Request &req) { (void)req; }

// * Status code description
std::string Response::statusCodeDescription(STATUS_CODE statusCode) {
  if (statusCode == this->OK) {
    return "200 OK";
  } else if (statusCode == this->CREATED) {
    return "201 Created";
  } else if (statusCode == this->NO_CONTENT) {
    return "204 No Content";
  } else if (statusCode == this->MOVED_PERMANENTLY) {
    return "301 Moved Permanently";
  } else if (statusCode == this->FOUND) {
    return "302 Found";
  } else if (statusCode == this->BAD_REQUEST) {
    return "400 Bad Request";
  } else if (statusCode == this->FORBIDDEN) {
    return "403 Forbidden";
  } else if (statusCode == this->NOT_FOUND) {
    return "404 Not Found";
  } else if (statusCode == this->METHOD_NOT_ALLOWED) {
    return "405 Method Not Allowed";
  } else if (statusCode == this->PAYLOAD_TOO_LARGE) {
    return "413 Payload Too Large";
  } else if (statusCode == this->INTERNAL_SERVER_ERROR) {
    return "500 Internal Server Error";
  } else if (statusCode == this->BAD_GATEWAY) {
    return "502 Bad Gateway";
  } else if (statusCode == this->GATEWAY_TIMEOUT) {
    return "504 Gateway Timeout";
  }
  return "Unknown Status";
}

// * count the body length
size_t Response::countBodyLength(const std::string &path) {
  struct stat buffer;

  if (stat(path.c_str(), &buffer) == -1) {
    return 0;
  }

  return buffer.st_size;
}

// * parse form URL encoded
std::map<std::string, std::string>
Response::parseFormURLEncoded(const std::string &post_body) {
  std::map<std::string, std::string> result;

  std::stringstream ss(post_body);
  std::string line;

  while (std::getline(ss, line, '&')) {
    size_t pos = line.find("=");

    if (pos == std::string::npos) {
      continue;
    }

    std::string key = line.substr(0, pos);
    std::string value = line.substr(pos + 1);

    result[key] = value;
  }

  return result;
}

// * Method Not Allowed
void Response::methodNotAllowed(const Request &req) {
  // * set status code
  this->setStatusCode(METHOD_NOT_ALLOWED);

  // * full path
  std::string fullPath = "pages/errors/405.html";

  // * Generate response
  this->generateResponse(req, fullPath);
}

// * Generate response
void Response::generateResponse(const Request &req, std::string &path) {

  // * root directory
  std::string root("pages");

  // * application/x-www-form-urlencoded page
  std::string normalPostPath = "/post-request-data.html";

  // * multipart/form-data; boundary=----WebKitFormBoundary page
  std::string uploadPostPath = "/post-request-upload.html";
  std::string errorUploadPostPath = "/post-request-error-upload.html";

  // * status line
  this->setStatusLine(req.httpV, statusCodeDescription(getStatusCode()));

  // * Content Type
  this->setContentType(path);

  // ? redirection when we have a post method
  if (req.method == POST) {
    // * get content type to decide which response will send in post method
    std::string contentType =
        req.getRequest().count("Content-Type")
            ? req.getRequest().find("Content-Type")->second
            : "";

    // ? application/x-www-form-urlencoded
    if (contentType == "application/x-www-form-urlencoded") {
      path = (root + normalPostPath);
    } else if (contentType.substr(0, 52) == // ? multipart/form-data;
                                            // ? boundary=----WebKitFormBoundary
               "multipart/form-data; boundary=----WebKitFormBoundary") {
      std::string uploadBody = req.getRequest().count("post-body")
                                   ? req.getRequest().find("post-body")->second
                                   : "";
      // * check if the file is empty
      if (uploadBody.empty()) {
        path = (root + errorUploadPostPath);
      } else {
        path = (root + uploadPostPath);
      }
    }
  }

  // * Content Length
  std::cout << "-----------------------path of file------------------------"
            << std::endl;
  std::cout << path << std::endl;
  std::cout << "-----------------------path of file------------------------"
            << std::endl;
  this->setContentLength(this->countBodyLength(path));

  // * merge all headers
  this->setHeaders();

  // * get fd of body
  int fd = open(path.c_str(), O_RDONLY);
  this->setBodyFd(fd);

  std::cout << "-----------------------Headers------------------------"
            << std::endl;
  std::cout << getHeaders() << std::endl;
  std::cout << "-----------------------Headers------------------------"
            << std::endl;
}

// * Response
void Response::response(const Request &req) {
  if (req.method == GET) {
    std::cout << "=======GET=======" << std::endl;
    this->GET_METHOD(req);
  } else if (req.method == POST) {
    this->POST_METHOD(req);
    std::cout << "=======POST=======" << std::endl;
  } else if (req.method == DELETE) {
    this->DELETE_METHOD(req);
    std::cout << "=======DELETE=======" << std::endl;
  } else {
    this->methodNotAllowed(req);
    std::cout << "=======ELSE=======" << std::endl;
  }
}
