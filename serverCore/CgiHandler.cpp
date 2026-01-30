/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 10:49:39 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/30 16:18:15 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiHandler.hpp"
#include <wait.h>


char*	CgiHandler::getBinaryPath(const Request& req)
{
	size_t dotPos = req.cgi.scriptPath.rfind('.');
	if (dotPos == std::string::npos)
		return NULL;

	std::string ext = req.cgi.scriptPath.substr(dotPos);
	std::map<std::string, std::string>::const_iterator it = req.config.cgi_conf.find(ext);

	if (it == req.config.cgi_conf.end())
		return NULL;

	return strdup(it->second.c_str());
}


char**	CgiHandler::buildArguments(const Request& req)
{
	char* interpreter = getBinaryPath(req);
	if (!interpreter)
		return NULL;

	char** argv = new char*[3];
	argv[0] = interpreter;
	argv[1] = strdup(req.cgi.scriptPath.c_str());
	argv[2] = NULL;

	return argv;
}


char**	CgiHandler::buildEnvVariables(const Request& req)
{
	std::vector<std::string> envVect;

	envVect.push_back("REQUEST_METHOD=" + req.cgi.method);
	envVect.push_back("SCRIPT_NAME=" + req.cgi.scriptPath);
	envVect.push_back("SERVER_PROTOCOL=HTTP/1.1");
	envVect.push_back("GATEWAY_INTERFACE=CGI/1.1");
	envVect.push_back("SERVER_SOFTWARE=500_Service_Unavailable/1.0");
	envVect.push_back("SERVER_NAME=" + req.cgi.host);
	envVect.push_back("SERVER_PORT=" + req.cgi.port);

	// check for query here
	if (!req.cgi.query.empty())
		envVect.push_back("QUERY_STRING=" + req.cgi.query);

	if (!req.cgi.pathInfo.empty())
		envVect.push_back("PATH_INFO=" + req.cgi.pathInfo);

	// check for method if post add the required headers
	if (req.cgi.method == "POST")
	{
		envVect.push_back("CONTENT_TYPE=" + req.cgi.contentType);
		envVect.push_back("CONTENT_LENGTH=" + toString(req.cgi.contentLength));
	}


	std::map<std::string, std::string>				headers = req.cgi.headers;
	std::map<std::string, std::string>::iterator	it = headers.begin();
	for (; it != headers.end(); ++it)
	{
		std::string	key = "HTTP_" + it->first;
		std::replace(key.begin(), key.end(), '-', '_');
		envVect.push_back(key + "=" + it->second);
	}
	

	char**	envp = new char*[envVect.size() + 1];
	for (size_t i = 0; i < envVect.size(); i++)
	{
		envp[i] = strdup(envVect[i].c_str());
	}
	envp[envVect.size()] = NULL;
	
	return (envp);
}

void	CgiHandler::executeScript(int stdinReadEnd, int stdoutWriteEnd, char **argv, char **envp)
{
	dup2(stdinReadEnd, STDIN_FILENO);
	dup2(stdoutWriteEnd, STDOUT_FILENO);

	close(stdoutWriteEnd);
	close(stdinReadEnd);
	
	execve(argv[0], argv, envp);

	throwError("execve()");
	_exit(127);
}

int	CgiHandler::startCgiScript(const Request& req, pid_t& outPid)
{
	int stdinPipe[2];
	int stdoutPipe[2];
	char **argv = NULL;
	char **envp = NULL;

	if (pipe(stdinPipe) < 0)
		throwError("pipe(stdin)");

	if (pipe(stdoutPipe) < 0)
		throwError("pipe(stdout)");

	setNonBlocking(stdoutPipe[0]);
		
	argv = buildArguments(req);
	envp = buildEnvVariables(req);
	
	pid_t	pid = fork();
	if (pid < 0)
	{
		close(stdoutPipe[0]);
		close(stdoutPipe[1]);
		close(stdinPipe[0]);
		close(stdinPipe[1]);
		throwError("fork()");
	}
	
	if (pid == 0)
	{
		close(stdinPipe[1]);
		close(stdoutPipe[0]);
		executeScript(stdinPipe[0], stdoutPipe[1], argv, envp);
		_exit(127);
	}
	close(stdinPipe[0]);
	close(stdoutPipe[1]);

	if (req.cgi.method == "POST")
	{
		write(stdinPipe[1], req.cgi.body.c_str(), req.cgi.body.size());
	}
	close(stdinPipe[1]);

	cleanUpArguments(argv);
	cleanUpEnvVariables(envp);

	outPid = pid;
	return (stdoutPipe[0]);
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

void	CgiHandler::cleanUpArguments(char **argv)
{
	if (!argv)
		return ;
	for (int i = 0; argv[i] != NULL; i++)
		free(argv[i]);
	delete[] argv;
}

void	CgiHandler::cleanUpEnvVariables(char **envp)
{
	if (!envp)
		return ;
	
	for (int i = 0; envp && envp[i] != NULL; i++)
		free(envp[i]);
	delete[] envp;
}