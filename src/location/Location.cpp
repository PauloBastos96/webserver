/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 15:46:15 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:03:45 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"

Location::Location()
{
	_config.setAutoIndex(false);
}
Location::~Location(){}

const string	&Location::getPath(void)
{
	return (_path);
}

ServerConfig	&Location::getConfig(void)
{
	return (_config);
}

void	Location::setPath(const string &path)
{
	_path = path;
}

vector<string>	&Location::getAllowedMethods(void)
{
	return (_allowedMethods);
}

void	Location::setAllowedMethods(const string &method)
{
	_allowedMethods.push_back(method);
}