/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 07:04:05 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/26 11:19:39 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Webserv.hpp"

class Helper {
	
	public:
		void				setNonBlocking(int fd);
		void				throwError(const std::string& sysCallFunc);
		static std::string	readFile(const std::string& path);
		static std::string	toString(size_t val);

};