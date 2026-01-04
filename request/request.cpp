/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   request.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/24 20:48:07 by mdahani           #+#    #+#             */
/*   Updated: 2026/01/04 18:56:57 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/webserv.hpp"

// * Setters & Getters
void Request::setRequest(const std::string &req) {
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

  std::string contentType =
      this->request.count("Content-Type") ? this->request["Content-Type"] : "";
  if (contentType.substr(0, 52) ==
      "multipart/form-data; boundary=----WebKitFormBoundary") {
    // * skip boundary
    ss.clear();
    ss << this->request["post-body"];

    if (!std::getline(ss, line)) {
      std::cerr << "boundary not found" << std::endl;
      return;
    }

    // * check info of post-body
    while (std::getline(ss, line)) {
      // * get file name
      pos = line.find("Content-Disposition");
      if (pos != std::string::npos) {
        pos = line.rfind("filename=\"");
        if (pos == std::string::npos) {
          std::cerr << "filename=\" not found" << std::endl;
          return;
        }
        size_t start = pos + 10;
        size_t end = line.rfind("\"");
        value = line.substr(start, end - start);
        key = "filename";
        break;
        ;
      }
    }

    // * check if not found Content-Disposition
    if (pos == std::string::npos) {
      std::cerr << "Content-Disposition not found" << std::endl;
      return;
    }

    // * get binary data of file
    pos = this->request["post-body"].find("\r\n\r\n");
    if (pos == std::string::npos) {
      std::cerr << "binary data of file not found" << std::endl;
      return;
    }

    // * find last boundary
    size_t end =
        this->request["post-body"].rfind("\r\n------WebKitFormBoundary");
    if (end == std::string::npos) {
      std::cerr << "last boundary not found" << std::endl;
      return;
    }

    key = "binary-data";
    size_t start = pos + 4;
    value = this->request["post-body"].substr(start, end - start);

    if (!value.empty()) {
      this->request[key] = value;
    } else {
      // * handle the empty file
      // * when you upload a non empty file and after that you remove all data
      // * and get it again
      this->request.erase(key);
    }
  }
}

const std::map<std::string, std::string> &Request::getRequest() const {
  return this->request;
}