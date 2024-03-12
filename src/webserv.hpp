/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 14:41:01 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 15:05:12 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define RED "\033[1;31m"
# define GREEN "\033[1;32m"
# define YELLOW "\033[1;33m"
# define BLUE "\033[1;34m"
# define RESET "\033[0m"

# include <iostream>
# include <string>
# include <vector>
# include "server/Server.hpp"
# include "logger/logger.hpp"
# include "config/config.hpp"
# include "AConfigParser/AConfigParser.hpp"

using std::string;
using std::cout;
using std::cerr;
using std::endl;

#endif