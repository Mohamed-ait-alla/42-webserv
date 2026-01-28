/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 07:06:49 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/28 17:11:43 by mait-all         ###   ########.fr       */
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
	logMessage(LOG_ERROR, "  " + sysCallFunc + " has been failed");
	throw std::runtime_error(sysCallFunc + " failed: " + strerror(errno));
}

std::string	Helper::readFile(const std::string& path)
{
	std::ifstream file(path.c_str(), std::ios::in | std::ios::binary);

	if (!file.is_open())
		throw std::runtime_error("Failed to open file: " + path);

	std::ostringstream ss;
	ss << file.rdbuf();

	return ss.str();
}

std::string	Helper::toString(size_t val)
{
	std::stringstream	ss;

	ss << val;
	return (ss.str());
}