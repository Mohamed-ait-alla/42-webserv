/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Helper.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mait-all <mait-all@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/15 07:04:05 by mait-all          #+#    #+#             */
/*   Updated: 2026/01/15 09:23:00 by mait-all         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <errno.h>

class Helper {
	
	public:
		void	setNonBlocking(int fd);
		void	throwError(const std::string& sysCallFunc);

};