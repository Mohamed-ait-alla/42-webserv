/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 10:43:47 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/24 09:42:57 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ConnectionManager.hpp"

#define CGI_TIMEOUT 5


class CgiHandler : public Helper {

	public:
		
		char**	buildArguments(const Request& req);
		char**	buildEnvVariables(const Request& req);
		void	executeScript(int pipeWriteEnd, char **argv, char **envp);
		int		startCgiScript(const Request& req, pid_t& outPid);
		ssize_t	readChunk(int pipeFd, char *buffer, size_t size);
		bool	checkCgiStatus(pid_t pid, int& exitStatus);
		std::string getMethodName(int enumFlag);

};