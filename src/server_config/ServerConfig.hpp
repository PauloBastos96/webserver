/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 14:01:35 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:12:51 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVERCONFIG_HPP
# define SERVERCONFIG_HPP

#include "../config_exception/ConfigException.hpp"
#include <vector>
#include <map>

using std::vector;
using std::map;

typedef unsigned int uint;

class ServerConfig
{
	private:
		string				_root;
		vector<string>		_index;
		map<uint, string>	_errorPage;
		string				_maxClientBodySize;
		bool				_autoIndex;
	public:
		ServerConfig();
		~ServerConfig();

		const string		&getRoot(void);
		vector<string>		&getIndex(void);
		map<uint, string>	&getErrorPage(void);
		const string		&getMaxClientBodySize(void);
		const bool			&getAutoIndex(void);

		void	setRoot(const string &root);
		void	setIndex(const string &index);
		void	setErrorPage(const uint &error_code, const string &error_page);
		void	setMaxClientBodySize(const string &max_client_body_size);
		void	setAutoIndex(const bool &auto_index);
};

#endif
