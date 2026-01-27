/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 05:51:20 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/27 12:54:39 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Client.hpp"
#include "Response.hpp"


class ConnectionManager : Helper {

	public:
		ConnectionManager(Epoll& epoll);
		
		void	setUpNewConnection(int serverFd, std::map<int, Client>& clients);
		bool	isCompleteRequest(const std::string &request);
		size_t	getContentLength(const std::string &request);
		bool	receiveData(int clientFd, std::map<int, Client>& clients, std::map<int, int>& cgiPipeToClient);
		bool	sendData(int clientFd, std::map<int, Client>& clients, std::map<int, int>& cgiPipeToClient, Request& req);
		void	closeConnection(int clientFd, std::map<int, Client>& clients, std::map<int, int>& cgiPipeToClient);

	private:
		Epoll	_epollInstance;

};