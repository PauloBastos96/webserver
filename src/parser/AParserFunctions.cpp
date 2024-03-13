#include "AParserFunctions.hpp"
#include <sstream>
#include <fstream>
#include <cstdlib>

AParserFunctions::AParserFunctions(void){}
AParserFunctions::~AParserFunctions(void){}

/// @brief Parse the IP address from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::host(const std::string line, server &server)
{
	std::string		host;
	if (line.find_first_of(".") != std::string::npos)
	{
		if (line.find_first_of(":") != std::string::npos)
			host = line.substr(line.find_first_of(" ") + 1, line.find_first_of(":") - line.find_first_of(" ") - 1);
		else
			host = line.substr(line.find_first_of(" ") + 1);
	}
	else
	{
		if (line.find_first_of(":") != std::string::npos)
			host = line.substr(line.find_first_of(" ") + 1, line.find_first_of(":") - line.find_first_of(" ") - 1);
		else
			host = line.substr(line.find_first_of(" ") + 1);
	}
	server.set_host(host);
}

/// @brief Parse the port from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::port(const std::string line, server &server)
{
	int	port = 80;

	if (line.find_first_of(":") != std::string::npos)
		port = std::atoi(line.substr(line.find_first_of(":") + 1).c_str());
	server.set_port(port);
}

/// @brief Parse the server name from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::server_name(const std::string line, server &server)
{
	std::stringstream ss(line);
	std::string word;
	while (ss >> word)
	{
		if (word != "server_name")
			server.set_server_name(word);
	}
}

/// @brief Parse the index from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void	AParserFunctions::index(const std::string line, server &server)
{
	server.get_config().getIndex().clear();
	std::stringstream ss(line);
	std::string word;
	while (ss >> word)
	{
		if (word != "index")
			server.get_config().setIndex(word);
	}
}

/// @brief Parse location block from the config file and add it to the server object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void	AParserFunctions::location(std::string line, server &server, int &line_number, std::ifstream &file)
{
	Location location;
	location.set_path(line.substr(line.find_first_of(" ") + 1, line.find_last_of(" " ) - line.find_first_of(" ") - 1));
	while (std::getline(file, line) && !file.eof() && line.find("}") == std::string::npos)
	{
		line_number++;
		if (line.find("root") != std::string::npos)
			location.get_config().setRoot(line.substr(line.find_first_of(" ") + 1));
		if (line.find("index") != std::string::npos)
			location.get_config().setIndex(line.substr(line.find_first_of(" ") + 1));
		if (line.find("error_page") != std::string::npos)
			if (parseErrorPage(line, location))
				WebServer::log("[CONFIG] Invalid error page directive", error);
		if (line.find("max_client_body_size") != std::string::npos)
			location.get_config().setMaxClientBodySize(line.substr(line.find_first_of(" ") + 1));
		if (line.find("location") != std::string::npos && line.end()[-1] == '{')
			WebServer::log("[CONFIG] Missing closing bracket", error);
		if (line.find("limit_except") != std::string::npos)
			limit_except(line, location);
		if (line.find("autoindex") != std::string::npos)
		{
			std::string autoindex = line.substr(line.find_first_of(" ") + 1);
			location.get_config().setAutoIndex(autoindex == "off" ? false : true);
		}
	}
	server.getlocations().push_back(location);
}

/// @brief Parse the limit_except block from the config file and add it to the location object
/// @param line The line from the config file
/// @param location The location object
void	AParserFunctions::limit_except(std::string line, Location &location)
{
	std::stringstream ss(line);
	std::string word;
	while (ss >> word)
	{
		if (word != "limit_except" && word.find_first_of("{") == std::string::npos)
			location.set_allowed_methods(word);
	}
}