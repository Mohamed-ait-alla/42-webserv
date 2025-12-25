/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 16:52:42 by mdahani           #+#    #+#             */
/*   Updated: 2025/12/25 09:26:44 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/webserv.hpp"

int main() {
  try {
    Server server;
    server.run();
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}