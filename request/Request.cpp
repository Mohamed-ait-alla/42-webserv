/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:48:07 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/19 11:17:03 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Request.hpp"

// * Default Constructor
Request::Request() : method(ELSE), path(""), httpV(""), isCGI(false) {
  // * generate a session_id for cookies
  srand(time(0));
  int sessionId = rand();

  // * change sessionId to str
  std::stringstream ss;
  ss << sessionId;
  this->setSession("session_id", ss.str());
}

// * Setters & Getters
void Request::setRequest(const std::string &req) {
  bool iFoundCookie = false;
  std::stringstream ss(req);

  // * get method & path & http version
  std::string line;
  if (std::getline(ss, line)) {
    std::stringstream firstLine(line);
    std::string reqMethod;
    firstLine >> reqMethod;
    if (reqMethod == "GET") {
      this->method = GET;
    } else if (reqMethod == "POST") {
      this->method = POST;
    } else if (reqMethod == "DELETE") {
      this->method = DELETE;
    } else {
      this->method = ELSE;
    }

    firstLine >> this->path >> this->httpV;

    // ! check if request is CGI
    this->checkCGI(this->path);
    if (this->getIsCGI()) {
      return; // ! if is CGI we don't need to build request just need path
    }
  }

  // * get the headers
  std::string key, value;
  size_t pos;
  while (std::getline(ss, line)) {
    pos = line.find(":");
    if (pos == std::string::npos) {
      break;
    }

    key = line.substr(0, pos);
    value = line.substr(pos + 2, line.length());

    // * check if client send cookies
    if (key == "Cookie") {
      iFoundCookie = true;
    }

    this->request[key] = value;
  }

  // * get the body
  pos = req.find("\r\n\r\n");
  if (pos == std::string::npos) {
    return;
  }

  value = req.substr(pos + 4, req.length());

  if (!value.empty()) {
    key = "post-body";
    this->request[key] = value;
  }

  // * CORRECT

  std::string contentType =
      this->request.count("Content-Type") ? this->request["Content-Type"] : "";
  if (contentType.substr(0, 52) ==
      "multipart/form-data; boundary=----WebKitFormBoundary") {

    // * check info of post-body (get file name)
    // * get file name
    pos = this->request["post-body"].find("Content-Disposition: ");
    // * check if not found Content-Disposition
    if (pos == std::string::npos) {
      std::cerr << "Content-Disposition not found" << std::endl;
      return;
    }

    pos = this->request["post-body"].find("filename=\"", pos);
    while ((pos = this->request["post-body"].find("filename=\"", pos)) !=
           std::string::npos) {
      if (this->request["post-body"][pos - 1] != '"') {
        break;
      }
      pos++;
    }

    if (pos == std::string::npos) {
      std::cerr << "filename not found" << std::endl;
      return;
    }

    size_t start = pos + 10;
    size_t end = this->request["post-body"].find("\"", start);
    key = "filename";
    value = this->request["post-body"].substr(start, end - start);
    this->request[key] = value;

    // * get binary data of file
    pos = this->request["post-body"].find("\r\n\r\n", end);
    if (pos == std::string::npos) {
      std::cerr << "binary data of file not found" << std::endl;
      return;
    }

    // * find last boundary
    end = this->request["post-body"].rfind("\r\n------WebKitFormBoundary");
    if (end == std::string::npos) {
      std::cerr << "last boundary not found" << std::endl;
      return;
    }

    key = "binary-data";
    start = pos + 4;
    value = this->request["post-body"].substr(start, end - start);

    if (!value.empty()) {
      this->request[key] = value;
    } else {
      // * handle the empty file when you upload a non empty file and after that
      // * you remove all data and get it again
      this->request.erase(key);
    }
  }

  // ! remove Cookies from request (because when the user remove it from browser
  // ! it still store it in request map)
  if (!iFoundCookie) {
    this->request.erase("Cookie");
  }

  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
  std::cout << this->isCGI << std::endl;
  std::cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << std::endl;
}

const std::map<std::string, std::string> &Request::getRequest() const {
  return this->request;
}

// * Session

void Request::setSession(const std::string session_id,
                         const std::string value) {
  this->session[session_id] = value;
}

const std::map<std::string, std::string> &Request::getSession() const {
  return this->session;
}

void Request::checkCGI(std::string path) {
  // * remove the first slash to check the file
  if (path[0] == '/') {
    path.erase(0, 1);
  }

  // * check the request is cgi
  size_t pos = path.find("/");
  if (pos == std::string::npos || path.substr(0, pos) != "cgi-bin") {
    this->isCGI = false;
    return;
  }

  // * check if file is in cgi-bin folder
  if (access(path.c_str(), F_OK) == -1 || !this->pathGCIisFile(path)) {
    this->isCGI = false;
  } else if (access(path.c_str(), R_OK) == -1 ||
             access(path.c_str(), W_OK) == -1) {
    this->isCGI = false;
  } else {
    this->isCGI = true;
  }
}

const bool &Request::getIsCGI() const { return this->isCGI; }

// * check if a file
bool Request::pathGCIisFile(std::string path) {
  struct stat buffer;

  // * get all status of path (size, type, ...)
  if (stat(path.c_str(), &buffer) == -1) {
    return false;
  }

  // * check the path is file or not
  if (S_ISREG(buffer.st_mode)) {
    return true;
  }

  return false;
}
