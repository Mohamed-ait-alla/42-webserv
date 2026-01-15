/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 07:06:49 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/15 07:08:35 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Helper.hpp"


void	Helper::setNonBlocking(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (flags == -1)
		throwError("fcntl(F_GETFL)");
	if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
		throwError("fcntl(F_SETFL)");
}

void	Helper::throwError(const std::string& sysCallFunc)
{
	 throw std::runtime_error(sysCallFunc + "failed: " + strerror(errno));
}