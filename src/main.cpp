#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include "logger/logger.h"
#include "config/config.h"
#include "server/Server.hpp"
#include "AParserFunctions/AParserFunctions.hpp"

using std::string;
using std::vector;
using std::cout;
using std::endl;

#define RESET   "\033[0m"

void	parseConfigFile(const string &path, vector<Server> &servers)
{
	std::ifstream file(path.c_str());
	if (!file.is_open())
	{
		throw ConfigException("Error: Could not open file " + path);
	}
	string line;
	int	line_number = 0;
	while (std::getline(file, line) && !file.eof())
	{
		line_number++;
		if (line.find("server") != string::npos && line.end()[-1] == '{')
		{
			Server server;
			while (std::getline(file, line) && !file.eof() && line.find("}") == string::npos)
			{
				line_number++;
				if (line.find("listen") != string::npos)
				{
					AParserFunctions::parseHost(line, server);
					AParserFunctions::parsePort(line, server);
				}
				if (line.find("server_name") != string::npos)
					AParserFunctions::parseServerName(line, server);
				if (line.find("root") != string::npos)
					server.getConfig().setRoot(line.substr(line.find_first_of(" ") + 1));
				if (line.find("index") != string::npos)
					AParserFunctions::parseIndex(line, server);
				if (line.find("error_page") != string::npos)
				{
					if (AParserFunctions::parseErrorPage(line, server))
						throw ConfigException("Error: Invalid error page directive", line_number);
				}
				if (line.find("max_client_body_size") != string::npos)
					server.getConfig().setMaxClientBodySize(line.substr(line.find_first_of(" ") + 1));
				if (line.find("location") != string::npos && line.end()[-1] == '{')
					AParserFunctions::parseLocation(line, server, line_number, file);
				if (line.find("autoindex") != string::npos)
				{
					string autoindex = line.substr(line.find_first_of(" ") + 1);
					server.getConfig().setAutoIndex(autoindex == "off" ? false : true);
				}
			}
			if (line.find("}") == string::npos)
			{
				throw ConfigException("Error: Missing closing bracket", line_number);
			}
			servers.push_back(server);
		}
	}
	file.close();
}

void	debugDisplayServerConfigs(vector<Server> &servers)
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

int main(const int ac, const char **av) {
    (void) av;
    try {
        logger logger;
        if (ac > 2)
            logger::log("webserv: too many arguments", error);
        // Parse the configuration file
        Config config(ac == 1 ? "configs/default.conf" : av[1]);
        // Setup the server
        // ...
        // while (true) {
            // Use poll() to handle multiple client connections
            // ...

            // For each client connection
            // ...

            // Read the HTTP request
            // ...

            // Process the request
            // ...

            // Generate the HTTP response
            // ...

            // Send the response
            // ...

            // End of each client connection
        // }

		vector<Server> servers;
		if (ac == 2)
		{
			parseConfigFile(av[1], servers);
		}
		else
		{
			parseConfigFile("configs/default.conf", servers);
		}
		debugDisplayServerConfigs(servers);
    } catch (...) {
        return 1;
    }
}