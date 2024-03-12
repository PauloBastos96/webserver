/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 12:31:24 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:04:33 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include "../server_config/ServerConfig.hpp"
#include "../location/Location.hpp"
#include <vector>

using std::vector;

class Server
{
	private:
		string			_host;
		int				_port;
		vector<string>	_serverName;
		ServerConfig	_config;
		vector<Location> _locations;
	public:
		Server(void);
		~Server(void);

		const int				&getPort(void);
		const string			&getHost(void);
		const vector<string>	&getServerName(void);
		ServerConfig			&getConfig(void);
		vector<Location>		&getLocations(void);

		void	setHost(const string &host);
		void	setPort(const int &port);
		void	setServerName(const string &server_name);
};

#endif