/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 11:18:05 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/26 13:44:05 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Webserv.hpp"


typedef struct location {
    std::string path;
    std::vector<std::string> allow_methods;
    bool autoindex;
    std::string root;
    std::string return_to;
    std::string index;
} location;


// ****************************************************************************** //
//                               ConfigFile Class                                 //
// ****************************************************************************** //

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


// ****************************************************************************** //
//                                 Request Class                                  //
// ****************************************************************************** //

class Request : public Webserv {

  private:
    std::map<std::string, std::string> request;
    std::map<std::string, std::string> session;
	std::string	_cgiResponse;

  public:
    METHOD method;
    std::string path;
    std::string httpV;
    bool isCGI;
    ConfigFile config;

    // * CGI information
    struct CgiInfo {
      CgiInfo();
  
      std::string host;
      std::string listen;
      METHOD method;
      std::string scriptPath;
      std::string pathInfo;
      // std::map<std::string,std::string> queries;
      std::string query;
      std::map<std::string,std::string> headers;
      std::string body;
      bool isChunked;
      size_t contentLength;
      std::string contentType;
    };

    // * obj of cgi struct
    CgiInfo cgi;


    // * Default Contructor
    Request();

    void setRequest(const std::string &req);
    const std::map<std::string, std::string> &getRequest() const;

    void setSession(const std::string session_id, const std::string value);
    const std::map<std::string, std::string> &getSession() const;
    
    void checkCGI(std::string path);
    const bool &getIsCGI() const;

    bool pathGCIisFile(std::string path);
	void	setCgiResponse(const std::string& cgiResponse);
	const std::string& getCgiResponse() const;
};
