/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/14 13:13:08 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/16 16:23:34 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Epoll.hpp"

class Client {

	public:
		Client();
		Client(int clientFd);
		// ~Client();

		void				appendRequest(const std::string& data, size_t length);
		size_t				findHeaderEnd() const;
		bool				hasCompleteHeaders() const;
		bool				hasCompleteBody() const;

		void				setIsPostRequest(bool val);
		void				setRequestComplete(bool val);
		void				setHeaderSent(bool val);
		void				setContentLength(size_t length);
		void				setBodyFd(int fd);

		const std::string&	getRequest() const;
		bool				isHeaderSent() const;
		int					getBodyFd() const;

	private:
		std::string	_request;
		size_t		_bytesReceived;
		size_t		_contentLength;
		bool		_isPostRequest;
		bool		_requestComplete;
		bool		_isHeaderSent;
		int			_clientFd;
		int			_bodyFd;

};