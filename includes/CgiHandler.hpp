/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 10:43:47 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/22 18:52:35 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "ConnectionManager.hpp"


class CgiHandler : public Helper {

	public:
		// CgiHandler(const Request& request);
		// ~CgiHandler();
		
		char**	buildArguments(const Request& req);
		// bool	execute(const Request& req);
		void	executeScript(int pipeWriteEnd, char **argv, char **envp);
		// void	readOutput(int pipeReadEnd);
		int		startCgiScript(const Request& req, pid_t& outPid);
		ssize_t	readChunk(int pipeFd, char *buffer, size_t size);
		bool	checkCgiStatus(pid_t pid, int& exitStatus);
		

		int		getExitStatus() const;
		std::string	getCgiOutput() const;

	private:
		// const Request&	_request;
		std::string		_output;
		int				_exitStatus;

};