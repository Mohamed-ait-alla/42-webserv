/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 15:59:16 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/17 13:44:48 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Response.hpp"
#include "ConnectionManager.hpp"

// ****************************************************************************** //
//                                  Server Class                                  //
// ****************************************************************************** //

class Server : Helper {

	public:
		Server();
		~Server();

		void	run(Request &req);
	
	private:
		std::map<int, Client>	_clients;
		Listener				_listener;
		Epoll					_epoll;
		ConnectionManager		_connectionManager;

		// private helper methods
		void	checkClientTimeOut();
		
		bool	receiveRequest(int clientFd);
		bool	sendResponse(int clientFd, Request& req);
		void	handleError(int clientFd);

		void	processClientEvent(int fd, struct epoll_event &ev, Request &req);
		void	processServerEvent(int fd);
};
