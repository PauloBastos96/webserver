/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 15:30:04 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:14:19 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(void)
{
	_port = 80;
	_host = "127.0.0.1";
	_config.setRoot("/");
	_config.setMaxClientBodySize("1m");
	_config.setIndex("index.html");
	_config.setAutoIndex(false);
}

Server::~Server(void){}

void	Server::setHost(const string &host)
{
	_host = host;
}
void	Server::setPort(const int &port)
{
	_port = port;
}
void	Server::setServerName(const string &server_name)
{
	_serverName.push_back(server_name);
}

const int	&Server::getPort(void)
{
	return (_port);
}

const string	&Server::getHost(void)
{
	return (_host);
}

const vector<string>	&Server::getServerName(void)
{
	return (_serverName);
}

ServerConfig	&Server::getConfig(void)
{
	return (_config);
}

vector<Location>	&Server::getLocations(void)
{
	return (_locations);
}