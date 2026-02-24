/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 07:04:05 by mait-all          #+#    #+#             */
/*   Updated: 2026/02/04 13:35:09 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../Webserv.hpp"

class Helper {
	
	public:
		static std::string	readFile(const std::string& path);
		static std::string	toString(size_t val);
		void				setNonBlocking(int fd);
		void				throwError(const std::string& sysCallFunc);

};