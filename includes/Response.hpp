/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 11:17:46 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/15 11:28:12 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Request.hpp"


// ****************************************************************************** //
//                                 Response Class                                 //
// ****************************************************************************** //

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
