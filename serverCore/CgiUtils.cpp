/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CgiUtils.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/26 11:04:26 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/26 11:30:18 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CgiUtils.hpp"

std::string ft_trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos)
        return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::map<std::string, std::string>	parseCgiHeaders(const std::string& rawHeaders) {
    std::map<std::string, std::string> headers;
    std::istringstream stream(rawHeaders);
    std::string line;
    
    while (std::getline(stream, line)) {
        // Remove \r if present (handles both \n and \r\n)
        if (!line.empty() && line[line.length() - 1] == '\r') {
            line = line.substr(0, line.length() - 1);
        }
        
        // Skip empty lines
        if (line.empty())
            continue;
        
        // Find the colon separator
        size_t colon_pos = line.find(':');
        if (colon_pos == std::string::npos) {
            // Malformed header - skip it
            continue;
        }
        
        // Extract header name and value
        std::string header_name = ft_trim(line.substr(0, colon_pos));
        std::string header_value = ft_trim(line.substr(colon_pos + 1));
        
        // Convert header name to lowercase for case-insensitive comparison
        // (HTTP headers are case-insensitive)
        for (size_t i = 0; i < header_name.length(); i++) {
            header_name[i] = std::tolower(header_name[i]);
        }
        
        // Store the header
        if (!header_name.empty()) {
            headers[header_name] = header_value;
        }
    }
    
    return headers;
}

std::string	loadErrorPage(int statusCode)
{
	std::string	path;
	
	if (statusCode == 500)
		path = "./pages/errors/500.html";
	else if (statusCode == 504)
		path = "./pages/errors/504.html";
	else
		path = "./pages/503.html";

	return (Helper::readFile(path));
}

CgiResult	parseCgiOutput(const std::string& raw)
{
	CgiResult	result;

	size_t	headerEnd = raw.find("\r\n\r\n");
	size_t	sepSize = 4;

	if (headerEnd == std::string::npos)
	{
		headerEnd = raw.find("\n\n");
		sepSize = 2;
	}

	if (headerEnd != std::string::npos)
	{
		std::string	headersStr = raw.substr(0, headerEnd);
		result.headers = parseCgiHeaders(headersStr);
		result.body = raw.substr(headerEnd + sepSize);
	}
	else
		result.body = raw;
	if (result.headers.find("content-type") == result.headers.end())
	{
		 result.headers["Content-Type"] = "text/plain";
	}
	
	if (result.headers.find("content-length") == result.headers.end())
	{
		result.headers["Content-Length"] = Helper::toString(result.body.size());
	}

	return (result);
}

std::string	buildCgiResponse(int statusCode,
										const std::string& reason,
										const std::map<std::string, std::string>& headers,
										const std::string& body)
{
	std::ostringstream oss;

	oss << "HTTP/1.1 " << statusCode << " " << reason << "\r\n";

	for (std::map<std::string, std::string>::const_iterator it = headers.begin(); it != headers.end(); ++it)
	{
		oss << it->first << ": " << it->second << "\r\n";
	}
	oss << "Connection: close\r\n";
	oss << "\r\n";
	oss << body;

	return (oss.str());
}