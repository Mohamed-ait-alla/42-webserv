/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Epoll.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/13 10:46:40 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/28 17:12:24 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Epoll.hpp"

Epoll::Epoll()
	: _epollfd(-1)
{
	_epollfd = epoll_create(1024);
	if (_epollfd < 0)
		throwError("epoll_create()");
}

Epoll::~Epoll()
{
	if (_epollfd >= 0)
		close (_epollfd);
}


void	Epoll::addFd(int fd, uint32_t event)
{
	struct epoll_event ev;

	ev.events = event;
	ev.data.fd = fd;

	if (epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev) < 0)
		throwError("epoll_ctl(EPOLL_CTL_ADD)");

	logEpollAdd(fd);
}

void	Epoll::modFd(int fd, uint32_t events)
{
	struct epoll_event	ev;

	ev.events = events;
	ev.data.fd = fd;

	if (epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev) < 0)
		throwError("epoll_ctl(EPOLL_CTL_MOD)");

	logEpollMod(fd, events);
}

void	Epoll::delFd(int fd, const std::string& reason)
{
	if (epoll_ctl(_epollfd, EPOLL_CTL_DEL, fd, NULL) < 0)
		throwError("epoll_ctl(EPOLL_CTL_DEL)");

	logepollDel(fd, reason);
}

int	Epoll::wait(struct epoll_event *events, int maxEvents)
{
	int nfds = epoll_wait(_epollfd, events, maxEvents, EPOLL_TIMEOUT);
	if (nfds < 0)
		throwError("epoll_wait()");
	return (nfds);
}

int	Epoll::getEpollFd() const
{
	return (_epollfd);
}

void	Epoll::logEpollAdd(int fd)
{
	logMessage(LOG_EPOLL, "  fd=" + toString(fd) + " added");
}

void	Epoll::logEpollMod(int fd, uint32_t event)
{
	if (event == EPOLLIN)
		logMessage(LOG_EPOLL, "  fd=" + toString(fd) + " modified (EPOLLIN)");
	if (event == EPOLLOUT)
		logMessage(LOG_EPOLL, "  fd=" + toString(fd) + " modified (EPOLLOUT)");
}

void	Epoll::logepollDel(int fd, const std::string& reason)
{
	logMessage(LOG_EPOLL, "  fd=" + toString(fd) + " removed (" + reason + ")");
}