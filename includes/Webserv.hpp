/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 11:48:16 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/26 18:42:44 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include <netinet/in.h>
#include <sstream>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include <vector>
#include <map>
#define PORT 8080
#define IP INADDR_ANY
#define IPv4 AF_INET
#define MAX_BUFFER_SIZE 4096
#define MAX_EVENTS 1024


// ****************************************************************************** //
//                                 werbserv Class                                 //
// ****************************************************************************** //

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
      SOMTHING_WENT_WRONG = 503,
      GATEWAY_TIMEOUT = 504
    };
};