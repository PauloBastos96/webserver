/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParserFunctions.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:11:49 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:26:25 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PARSERFUNCTIONS_HPP
# define PARSERFUNCTIONS_HPP

#include "../server/Server.hpp"
#include <sstream>
#include <cstdlib>

using std::string;

class AParserFunctions
{
	private:
		AParserFunctions(void);
		virtual ~AParserFunctions(void) = 0;
	public:
		static void	parseHost(const string line, Server &server);
		static void	parsePort(const string line, Server &server);
		static void	parseServerName(const string line, Server &server);
		static void	parseIndex(const string line, Server &server);
		static void	parseLocation(string line, Server &server, int &line_number, std::ifstream &file);
		static void	parseLimitExcept(string line, Location &location);
		
		template <typename T>
		static int	parseErrorPage(const string line, T &obj)
		{
			std::stringstream ss(line);
			string word;
			while (ss >> word)
			{
				if (word != "error_page" && word.find_first_of("/") == string::npos)
				{
					if (word.find_first_not_of("0123456789") != string::npos)
						return (1);
					obj.getConfig().setErrorPage(std::atoi(word.c_str()), ss.str().substr(ss.str().find_first_of("/")));
				}
			}
			return (0);
		}
};

#endif