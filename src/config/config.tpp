#ifndef CONFIG_TPP
#define CONFIG_TPP

#include "webserver.hpp"
#include "logger.hpp"

/// @brief Parse error pages from the configuration file
/// @tparam T Supports server and location objects
/// @param line The line to parse
/// @param obj The server or location object
template<typename T>
static void parse_error_page(const std::string &line, T &obj) {
	std::stringstream ss(line);
	std::string word;
	std::string errorPage;
	int	errorCode;
	if (ss.str().find_first_of('/') == std::string::npos)
		WebServer::log(ERR_CFG_INVALID_ERROR_PAGE, error);
	while (ss >> word) {
		if (word != "error_page" && word.find_first_of('/') == std::string::npos) {
			if (word.find_first_not_of("0123456789") != std::string::npos)
				WebServer::log(ERR_CFG_INVALID_ERROR_PAGE, error);
			errorCode = std::atoi(word.c_str());
			errorPage = ss.str().substr(ss.str().find_first_of("/"), ss.str().find_first_of(';') - ss.str().find_first_of('/'));
			obj.get_config().set_error_page(errorCode, errorPage);
		}
	}
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