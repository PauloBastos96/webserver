/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ServerConfig.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/19 14:14:43 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:04:07 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ServerConfig.hpp"

ServerConfig::ServerConfig(){}
ServerConfig::~ServerConfig() {}

void	ServerConfig::setRoot(const std::string &root)
{
	_root = root;
}
void	ServerConfig::setIndex(const std::string &index)
{
	_index.push_back(index);
}
void	ServerConfig::setErrorPage(const uint &error_code, const std::string &error_page)
{
	_errorPage.insert(std::pair<uint, std::string>(error_code, error_page));
}
void	ServerConfig::setMaxClientBodySize(const std::string &max_client_body_size)
{
	_maxClientBodySize = max_client_body_size;
}

const std::string	&ServerConfig::getRoot(void)
{
	return (_root);
}
vector<std::string>	&ServerConfig::getIndex(void)
{
	return (_index);
}
map<uint, std::string>	&ServerConfig::getErrorPage(void)
{
	return (_errorPage);
}
const std::string	&ServerConfig::getMaxClientBodySize(void)
{
	return (_maxClientBodySize);
}

void	ServerConfig::setAutoIndex(const bool &autoindex)
{
	_autoIndex = autoindex;
}

const bool	&ServerConfig::getAutoIndex(void)
{
	return (_autoIndex);
}