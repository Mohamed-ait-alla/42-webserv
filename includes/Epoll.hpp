/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 10:46:25 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/14 11:56:55 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <sys/epoll.h>
#include "Listener.hpp"


class Epoll {

	public:
		void	createEpollInstance();
		void	addFd(int fd, uint32_t event);
		void	modFd(int fd, uint32_t event);
		void	delFd(int fd);
		int		wait(struct epoll_event *events, int maxEvents);

		int		getEpollFd() const;

	private:
		int	_epollfd;

};