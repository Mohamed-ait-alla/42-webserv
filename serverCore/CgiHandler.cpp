/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 10:49:39 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/21 18:55:26 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiHandler.hpp"
#include <wait.h>


char**	CgiHandler::buildArguments(const Request& req)
{
	char**	argv = new char*[3];
	std::string path = "." + req.path;

	argv[0] = strdup(req.config.cgi_path[0].c_str());
	argv[1] = strdup(path.c_str());
	argv[2] = NULL;

	return (argv);
}

void	CgiHandler::executeScript(int pipeWriteEnd, char **argv, char **envp)
{
	dup2(pipeWriteEnd, STDOUT_FILENO);
	// dup2(pipeFds[0], STDIN_FILENO);
	close(pipeWriteEnd);

	// close(pipeFds[0]);
	// close(pipeFds[1]);
	
	
	execve(argv[0], argv, envp);

	throwError("execve()");
	_exit(127);
}

// void	CgiHandler::readOutput(int pipeReadEnd)
// {
// 	char	buffer[4096];
// 	ssize_t	bytesRead;
	
// 	_output.clear();
	
// 	while ((bytesRead = read(pipeReadEnd, buffer, sizeof(buffer))) > 0)
// 	{
// 		_output.append(buffer, bytesRead);
// 	}
// }

// bool	CgiHandler::execute(const Request& req)
// {
// 	int		pipeFds[2];
// 	pid_t	pid;
// 	char**	argv = NULL;
// 	char**	envp = NULL;

// 	if (pipe(pipeFds) < 0)
// 		throwError("pipe()");
	
// 	argv = buildArguments(req);
// 	pid = fork();
// 	if (pid < 0)
// 	{
// 		close(pipeFds[0]);
// 		close(pipeFds[1]);
// 		throwError("fork()");
// 	}

// 	if (pid == 0)
// 	{
// 		executeScript(pipeFds, argv, envp);
// 		_exit(127);
// 	}

// 	close(pipeFds[1]);

// 	readOutput(pipeFds[0]);
// 	close(pipeFds[0]);

// 	// std::cout << "----output Readed----\n";
// 	// std::cout << _output << std::endl;

// 	int status;
// 	waitpid(pid, &status, 0);
// 	_exitStatus = status;

// 	return (_exitStatus);
// }


int	CgiHandler::startCgiScript(const Request& req, pid_t& outPid)
{
	int pipeFds[2];
	char **argv = NULL;
	char **envp = NULL;

	if (pipe(pipeFds) < 0)
		throwError("pipe()");

	setNonBlocking(pipeFds[0]);
		
	argv = buildArguments(req);
	
	pid_t	pid = fork();
	if (pid < 0)
	{
		close(pipeFds[0]);
		close(pipeFds[1]);
		throwError("fork()");
	}
	
	if (pid == 0)
	{
		close(pipeFds[0]);
		executeScript(pipeFds[1], argv, envp);
		_exit(127);
	}

	close(pipeFds[1]);

	outPid = pid;
	return (pipeFds[0]);
}

bool	CgiHandler::checkCgiStatus(pid_t pid, int& exitStatus)
{
	int	status;

	pid_t	result = waitpid(pid, &status, WNOHANG);
	
	if (result == 0)
		return (false);
	else if (result > 0)
	{
		exitStatus = WEXITSTATUS(status);
		return (true);
	}
	// case error
	return (false);
}

ssize_t	CgiHandler::readChunk(int pipeFd, char *buffer, size_t size)
{
	return (read(pipeFd, buffer, size));
}

int	CgiHandler::getExitStatus() const
{
	return (_exitStatus);
}

std::string	CgiHandler::getCgiOutput() const
{
	return (_output);
}