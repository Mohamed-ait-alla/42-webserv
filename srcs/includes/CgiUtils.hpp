/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiUtils.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 11:01:22 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/26 11:29:42 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once
#include "ConnectionManager.hpp"


struct CgiResult {
	std::map<std::string, std::string>	headers;
	std::string							body;
};

std::map<std::string, std::string>	parseCgiHeaders(const std::string& rawHeaders);
std::string							ft_trim(const std::string& str);
std::string							loadErrorPage(int statusCode);
CgiResult							parseCgiOutput(const std::string& raw);
std::string							buildCgiResponse(int statusCode, const std::string& reason,
									 					const std::map<std::string, std::string>& headers,
									 					const std::string& cgiOutput);