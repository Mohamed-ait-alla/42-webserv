/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConnectionManager.hpp                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 05:51:20 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/16 17:13:16 by mait-all         ###   ########.fr       */
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
		bool	receiveData(int clientFd, std::map<int, Client>& clients);
		bool	sendData(int clientFd, std::map<int, Client>& clients, Request& req);

	private:
		Epoll	_epollInstance;

};