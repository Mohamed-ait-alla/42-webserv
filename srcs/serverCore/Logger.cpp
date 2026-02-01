/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Logger.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/28 09:39:31 by mait-all          #+#    #+#             */
/*   Updated: 2026/02/01 13:15:58 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Logger.hpp"

bool debug_mode = false;

const char*	tagToStr(LogTag tag)
{
    if (tag == LOG_INFO)    return "INFO";
    if (tag == LOG_EPOLL)   return "EPOLL";
    if (tag == LOG_CONN)    return "CONN";
    if (tag == LOG_REQ)     return "REQ";
    if (tag == LOG_RESP)    return "RESP";
    if (tag == LOG_CLOSE)   return "CLOSE";
    if (tag == LOG_CGI)     return "CGI";
    if (tag == LOG_TIMEOUT) return "TIMEOUT";
	if (tag == LOG_DEBUG)   return "DEBUG";
    if (tag == LOG_ERROR)   return "ERROR";
    return "UNKN";
}

const char*	tagToColor(LogTag tag)
{
    if (tag == LOG_INFO)    return CLR_GREEN;
    if (tag == LOG_EPOLL)   return CLR_CYAN;
    if (tag == LOG_CONN)    return CLR_BLUE;
    if (tag == LOG_REQ)     return CLR_YELLOW;
    if (tag == LOG_RESP)    return CLR_GREEN;
    if (tag == LOG_CLOSE)   return CLR_GRAY;
    if (tag == LOG_CGI)     return CLR_MAGENTA;
    if (tag == LOG_ERROR)   return CLR_RED;
	if (tag == LOG_DEBUG)	return CLR_GRAY;
	if (tag == LOG_TIMEOUT) return CLR_BOLD_YELLOW;
    return CLR_RESET;
}

void	logMessage(LogTag tag, const std::string& msg)
{
	if (!debug_mode && tag != LOG_INFO)
		return ;

	std::cout
		<< tagToColor(tag)
		<< "[" << tagToStr(tag) << "] "
		<< msg
		<< CLR_RESET
		<< std::endl;
}