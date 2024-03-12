/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AConfigParser.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: paulorod <paulorod@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/02/20 16:17:05 by paulorod          #+#    #+#             */
/*   Updated: 2024/03/12 14:50:49 by paulorod         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AConfigParser.hpp"
#include "../webserv.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>

AConfigParser::AConfigParser(void){}
AConfigParser::~AConfigParser(void){}

static string	trim(const string &str)
{
	string result = str;
	size_t start = result.find_first_not_of(" \t");
	size_t end = result.find_last_not_of(" \t");
	if (start == string::npos || end == string::npos)
		return "";
	return result.substr(start, end - start + 1);
}

/**	@brief Parse the IP address from the line from the config file and set it to the server object
 *	@param line The line from the config file
 *	@param server The server object
 */
void	AConfigParser::parseHost(const string line, Server &server)
{
	string		host = "127.0.0.1"; //TODO replace with default value from MACRO
	if (line.find_first_of("=") != string::npos)
	{
		host = line.substr(line.find_first_of("=") + 1);
		host = trim(host);
		if (host.find_first_of(" ") != string::npos)
			logger::log("Invalid host directive", error);
	}
	server.setHost(host);
}

/**	@brief Parse the port from the line from the config file and set it to the server object
 *	@param line The line from the config file
 *	@param server The server object
 */
void	AConfigParser::parsePort(const string line, Server &server)
{
	int	port = 80;
	
	if (line.find_first_of("=") != string::npos)
		port = std::atoi(line.substr(line.find_first_of("=") + 1).c_str());
	server.setPort(port);
}

/**	@brief Parse the server name from the line from the config file and set it to the server object
 *	@param line The line from the config file
 *	@param server The server object
*/
void	AConfigParser::parseServerName(const string line, Server &server)
{
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "server_name" && word.find_first_of('=') == string::npos)
			server.setServerName(word);
	}
}

/**	@brief Parse the index from the line from the config file and set it to the server object
 *	@param line The line from the config file
 *	@param server The server object
 */
void	AConfigParser::parseIndex(const string line, Server &server)
{
	server.getConfig().getIndex().clear();
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "index" && word.find_first_of('=') == string::npos)
			server.getConfig().setIndex(word);
	}
}

/**	@brief Parse location block from the config file and add it to the server object
 *	@param line The line from the config file
 *	@param server The server object
 *	@param line_number The starting line number
 *	@param file The config file stream
*/
void	AConfigParser::parseLocation(string line, Server &server, int &line_number, std::ifstream &file)
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
				throw ConfigException("Invalid error page directive", line_number);
		}
		if (line.find("max_client_body_size") != string::npos)
			location.getConfig().setMaxClientBodySize(line.substr(line.find_first_of(" ") + 1));
		if (line.find("location") != string::npos && line.end()[-1] == '{')
			throw ConfigException("Missing closing bracket", line_number - 1);
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

/**	@brief Parse the limit_except block from the config file and add it to the location object
*	@param line The line from the config file
*	@param location The location object
*/
void	AConfigParser::parseLimitExcept(string line, Location &location)
{
	std::stringstream ss(line);
	string word;
	while (ss >> word)
	{
		if (word != "limit_except" && word.find_first_of("{") == string::npos)
			location.setAllowedMethods(word);
	}
}

vector<Server>	*AConfigParser::parseConfigFile(const string &path)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		throw ConfigException("Could not open file " + path);
	}
	string line;
	int	line_number = 0;
	vector<Server> *servers = new vector<Server>();
	while (std::getline(file, line) && !file.eof())
	{
		line_number++;
		if (line.find("[server]") != string::npos)
		{
			Server server;
			while (std::getline(file, line) && !file.eof() && line.find("[server]") == string::npos)
			{
				line_number++;
				if (line.find("host") != string::npos)
					AConfigParser::parseHost(line, server);
				if (line.find("port") != string::npos)
					AConfigParser::parsePort(line, server);
				if (line.find("server_name") != string::npos)
					AConfigParser::parseServerName(line, server);
				if (line.find("root") != string::npos)
				{
					string root = line.substr(line.find_first_of('=') + 1);
					root = trim(root);
					server.getConfig().setRoot(root);
				}
				if (line.find("index") != string::npos)
					AConfigParser::parseIndex(line, server);
				if (line.find("error_page") != string::npos)
				{
					if (AConfigParser::parseErrorPage(line, server))
						throw ConfigException("Invalid error page directive", line_number);
				}
				if (line.find("max_client_body_size") != string::npos)
					server.getConfig().setMaxClientBodySize(line.substr(line.find_first_of('=') + 1));
				if (line.find("location") != string::npos && line.end()[-1] == '{')
					AConfigParser::parseLocation(line, server, line_number, file);
				if (line.find("autoindex") != string::npos)
				{
					string autoindex = line.substr(line.find_first_of(" ") + 1);
					server.getConfig().setAutoIndex(autoindex == "off" ? false : true);
				}
			}
			servers->push_back(server);
		}
	}
	file.close();
	return (servers);
}

void	AConfigParser::debugDisplayServerConfigs(vector<Server> &servers)
{
	for (size_t i = 0; i < servers.size(); i++)
	{
		cout << BLUE << "Host " << servers[i].getHost() << ":" << servers[i].getPort() << RESET << endl;
		cout << "Server name: ";
		for (size_t j = 0; j < servers[i].getServerName().size() ; j++)
		{
			cout << servers[i].getServerName()[j] << " ";
		}
		cout << endl;
		cout << "Root: " << servers[i].getConfig().getRoot() << endl;
		cout << "Index: ";
		for (size_t j = 0; j < servers[i].getConfig().getIndex().size(); j++)
		{
			cout << servers[i].getConfig().getIndex()[j] << " ";
		}
		cout << endl;
		cout << "Error pages: ";
		for (std::map<uint, string>::iterator it = servers[i].getConfig().getErrorPage().begin(); it != servers[i].getConfig().getErrorPage().end(); it++)
		{
			cout << it->first << "=>" << it->second << " ";
		}
		cout << endl;
		cout << "Max client body size: " << servers[i].getConfig().getMaxClientBodySize() << endl;
		cout << "Autoindex: " << (servers[i].getConfig().getAutoIndex() ? "on" : "off") << endl;
		cout << "Locations: " << endl;
		for (size_t j = 0; j < servers[i].getLocations().size(); j++)
		{
			cout << "	" << servers[i].getLocations()[j].getPath() << endl;
			cout << "	" << "Root: " << servers[i].getLocations()[j].getConfig().getRoot() << endl;
			cout << "	" << "Index: ";
			for (size_t k = 0; k < servers[i].getLocations()[j].getConfig().getIndex().size(); k++)
			{
				cout << servers[i].getLocations()[j].getConfig().getIndex()[k];
			}
			cout << endl;
			cout << "	" << "Error pages: ";
			for (std::map<uint, string>::iterator it = servers[i].getLocations()[j].getConfig().getErrorPage().begin(); it != servers[i].getLocations()[j].getConfig().getErrorPage().end(); it++)
			{
				cout << "	" << it->first << " " << it->second << " ";
			}
			cout << endl;
			cout << "	" << "Max client body size: " << servers[i].getLocations()[j].getConfig().getMaxClientBodySize() << endl;
			cout << "	" << "Allowed methods: ";
			for (size_t k = 0; k < servers[i].getLocations()[j].getAllowedMethods().size(); k++)
			{
				cout << servers[i].getLocations()[j].getAllowedMethods()[k] << " ";
			}
			cout << endl;
			cout << "	" << "Autoindex: " << (servers[i].getLocations()[j].getConfig().getAutoIndex() ? "on" : "off") << endl;
			cout << endl;
		}
	}
}
