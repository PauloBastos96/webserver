#ifndef CONFIG_TPP
#define CONFIG_TPP

#include "webserver.hpp"
#include "logger.hpp"

template<typename T>
static int parse_error_page(const std::string &line, T &obj) {
	std::stringstream ss(line);
	std::string word;
	while (ss >> word) {
		if (word != "error_page" && word.find_first_of('/') == std::string::npos) {
			if (word.find_first_not_of("0123456789") != std::string::npos)
				return (1);
			obj.get_config().set_error_page(std::atoi(word.c_str()), ss.str().substr(ss.str().find_first_of("/")));
		}
	}
	return (0);
}

/// @brief Parse line with multiple values
/// @tparam T Only supports server objects
/// @param line The line to parse
/// @param server The server object
/// @param value The type of value to parse
template<typename T>
static void multi_value(const std::string &line, T &server, const std::string &value) {
	std::stringstream ss(line);
	std::string word;
	if (line.at(line.size() - 1) != ';')
		WebServer::log(WARM_CFG_SEMICOLON, warning);
	if (value == "index")
		server.get_config().get_index().clear();
	while (ss >> word) {
		if (word != value)
		{
			if (word.at(word.size() - 1) == ';')
				word = word.erase(word.size() - 1, 1);
			if (value == "index")
				server.get_config().set_index(word);
			else if (value == "server_name")
				server.set_server_name(word);
		}
	}
}

#endif