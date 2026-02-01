/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdahani <mdahani@student.1337.ma>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 16:52:42 by mdahani           #+#    #+#             */
/*   Updated: 2026/02/01 15:53:52 by mdahani          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./includes/Server.hpp"

int main(int ac, char **av) {

  try {
    Request request;
    Server server;

    if (ac <= 2) {
      char file[] = "config/default.conf";
      request.config.parse_config_file(file);
      if (ac == 2)
        request.config.parse_config_file(av[1]);
    } else
      throw std::runtime_error("./program [config file]");

    debug_mode = true;
    server.run(request);
  } catch (const std::exception &e) {
    std::cerr << e.what() << '\n';
    return 1;
  }
}