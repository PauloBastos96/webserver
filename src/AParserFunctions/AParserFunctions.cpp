/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AParserFunctions.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:17:05 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 13:23:34 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AParserFunctions.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>

AParserFunctions::AParserFunctions(void){}
AParserFunctions::~AParserFunctions(void){}

/// @brief Parse the IP address from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::parseHost(const string line, Server &server)
{
	string		host;
	if (line.find_first_of(".") != string::npos)
	{
		if (line.find_first_of(":") != string::npos)
			host = line.substr(line.find_first_of(" ") + 1, line.find_first_of(":") - line.find_first_of(" ") - 1);
		else
			host = line.substr(line.find_first_of(" ") + 1);
	}
	else
	{
		if (line.find_first_of(":") != string::npos)
			host = line.substr(line.find_first_of(" ") + 1, line.find_first_of(":") - line.find_first_of(" ") - 1);
		else
			host = line.substr(line.find_first_of(" ") + 1);
	}
	server.setHost(host);
}

/// @brief Parse the port from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::parsePort(const string line, Server &server)
{
	int	port = 80;
	
	if (line.find_first_of(":") != string::npos)
		port = std::atoi(line.substr(line.find_first_of(":") + 1).c_str());
	server.setPort(port);
}

/// @brief Parse the server name from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::parseServerName(const string line, Server &server)
{
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "server_name")
			server.setServerName(word);
	}
}

/// @brief Parse the index from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::parseIndex(const string line, Server &server)
{
	server.getConfig().getIndex().clear();
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "index")
			server.getConfig().setIndex(word);
	}
}

/// @brief Parse location block from the config file and add it to the server object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void	AParserFunctions::parseLocation(string line, Server &server, int &line_number, std::ifstream &file)
{
	Location location;
	location.setPath(line.substr(line.find_first_of(" ") + 1, line.find_last_of(" " ) - line.find_first_of(" ") - 1));
	while (std::getline(file, line) && !file.eof() && line.find("}") == string::npos)
	{
		line_number++;
		if (line.find("root") != string::npos)
			location.getConfig().setRoot(line.substr(line.find_first_of(" ") + 1));
		if (line.find("index") != string::npos)
			location.getConfig().setIndex(line.substr(line.find_first_of(" ") + 1));
		if (line.find("error_page") != string::npos)
		{
			if (parseErrorPage(line, location))
				throw ConfigException("Error: Invalid error page directive", line_number);
		}
		if (line.find("max_client_body_size") != string::npos)
			location.getConfig().setMaxClientBodySize(line.substr(line.find_first_of(" ") + 1));
		if (line.find("location") != string::npos && line.end()[-1] == '{')
			throw ConfigException("Error: Missing closing bracket", line_number - 1);
		if (line.find("limit_except") != string::npos)
			parseLimitExcept(line, location);
		if (line.find("autoindex") != string::npos)
		{
			string autoindex = line.substr(line.find_first_of(" ") + 1);
			location.getConfig().setAutoIndex(autoindex == "off" ? false : true);
		}
	}
	server.getLocations().push_back(location);
}

/// @brief Parse the limit_except block from the config file and add it to the location object
/// @param line The line from the config file
/// @param location The location object
void	AParserFunctions::parseLimitExcept(string line, Location &location)
{
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "limit_except" && word.find_first_of("{") == string::npos)
			location.setAllowedMethods(word);
	}
}
