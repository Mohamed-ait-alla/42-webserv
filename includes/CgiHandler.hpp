/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 10:43:47 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/28 10:59:22 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "CgiUtils.hpp"

#define CGI_TIMEOUT 5


class CgiHandler : public Helper {

	public:
		
		char**	buildArguments(const Request& req);
		char**	buildEnvVariables(const Request& req);
		void	executeScript(int stdinReadEnd, int stdoutWriteEnd, char **argv, char **envp);
		int		startCgiScript(const Request& req, pid_t& outPid);
		ssize_t	readChunk(int pipeFd, char *buffer, size_t size);
		bool	checkCgiStatus(pid_t pid, int& exitStatus);

};