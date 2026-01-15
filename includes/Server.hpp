/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 15:59:16 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/15 12:02:43 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Response.hpp"
#include "Listener.hpp"
#include "Epoll.hpp"
#include "Client.hpp"
#include "ConnectionManager.hpp"

// ****************************************************************************** //
//                                  Server Class                                  //
// ****************************************************************************** //

class Request;

class Server : Helper {
	private:
		Listener			_listener;
		Epoll				_epoll;
		ConnectionManager	_connectionManager;


	public:
		std::map<int, Client> clients;

		Server();
		// Server(std::string &host, int port);
		~Server();

		bool	isCompleteRequest(const std::string &request);
		size_t	getContentLength(const std::string &request);
		// void	setUpNewConnection(int serverFd);
		bool	recvRequest(int notifiedFd);
		bool	sendResponse(int notifiedFd, Request &request);
		void	processClientEvent(int fd, struct epoll_event &ev, Request &req);
		void	processServerEvent(int fd);
		void	run(Request &req);
};
