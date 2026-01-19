/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Request.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 11:18:05 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/19 10:50:41 by mdahani          ###   ########.fr       */
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

  public:
    METHOD method;
    std::string path;
    std::string httpV;
    bool isCGI;
    ConfigFile config;

    // * Default Contructor
    Request();

    void setRequest(const std::string &req);
    const std::map<std::string, std::string> &getRequest() const;

    void setSession(const std::string session_id, const std::string value);
    const std::map<std::string, std::string> &getSession() const;
    
    void checkCGI(std::string path);
    const bool &getIsCGI() const;

    bool pathGCIisFile(std::string path);
};