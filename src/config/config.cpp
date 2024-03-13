#include <iostream>
#include "config.hpp"
#include "../webserver/webserver.hpp"

Config::Config(const std::string &config_file) {
    file.open(config_file.c_str(), std::ios::in);
    if (!file)
        WebServer::log("Failed to open Config file", error);
    WebServer::log("Config file opened", info);
}

Config::~Config() {
    WebServer::log("Config file closed", info);
    file.close();
}

void Config::parse_config_file(const std::string &path, vector<server> &servers) {
    std::ifstream file(path.c_str());
    if (!file.is_open())
        WebServer::log("[CONFIG] Could not open file " + path, error);
    std::string line;
    int line_number = 0;
    while (std::getline(file, line) && !file.eof()) {
        line_number++;
        if (line.size() >= 2 && line.find("server") != std::string::npos && line.at(line.size() - 2) == '{') {
            server server;
            while (std::getline(file, line) && !file.eof() && line.find('}') == std::string::npos) {
                line_number++;
                if (line.find("listen") != std::string::npos) {
                    Config::host(line, server);
                    Config::port(line, server);
                }
                if (line.find("server_name") != std::string::npos)
                    Config::server_name(line, server);
                if (line.find("root") != std::string::npos)
                    server.get_config().setRoot(line.substr(line.find_first_of(' ') + 1));
                if (line.find("index") != std::string::npos)
                    Config::index(line, server);
                if (line.find("error_page") != std::string::npos) {
                    if (Config::parseErrorPage(line, server))
                        WebServer::log("[CONFIG] Invalid error page directive", error);
                }
                if (line.find("max_client_body_size") != std::string::npos)
                    server.get_config().setMaxClientBodySize(line.substr(line.find_first_of(' ') + 1));
                if (line.find("location") != std::string::npos && line.at(line.size() - 2) == '{')
                    Config::location(line, server, line_number, file);
                if (line.find("autoindex") != std::string::npos) {
                    std::string autoindex = line.substr(line.find_first_of(' ') + 1);
                    server.get_config().setAutoIndex(autoindex != "off");
                }
            }
            if (line.find('}') == std::string::npos) {
                WebServer::log("[CONFIG] Missing closing bracket", error);
            }
            WebServer::log("[CONFIG] Server configuration parsed", info);
            servers.push_back(server);
        }
    }
    file.close();
}

void Config::display_configs(vector<server> &servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << BLUE << "Host " << servers[i].get_hort() << ":" << servers[i].get_port() << std::endl;
        std::cout << "Server name: ";
        for (size_t j = 0; j < servers[i].get_server_name().size(); j++) {
            std::cout << servers[i].get_server_name()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Root: " << servers[i].get_config().getRoot() << std::endl;
        std::cout << "Index: ";
        for (size_t j = 0; j < servers[i].get_config().getIndex().size(); j++) {
            std::cout << servers[i].get_config().getIndex()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Error pages: ";
        for (std::map<uint, std::string>::iterator it = servers[i].get_config().getErrorPage().begin();
             it != servers[i].get_config().getErrorPage().end(); it++) {
            std::cout << it->first << "=>" << it->second << " ";
        }
        std::cout << std::endl;
        std::cout << "Max client body size: " << servers[i].get_config().getMaxClientBodySize() << std::endl;
        std::cout << "Autoindex: " << (servers[i].get_config().getAutoIndex() ? "on" : "off") << std::endl;
        std::cout << "Locations: " << std::endl;
        for (size_t j = 0; j < servers[i].getlocations().size(); j++) {
            std::cout << "	" << servers[i].getlocations()[j].get_path() << std::endl;
            std::cout << "	" << "Root: " << servers[i].getlocations()[j].get_config().getRoot() << std::endl;
            std::cout << "	" << "Index: ";
            for (size_t k = 0; k < servers[i].getlocations()[j].get_config().getIndex().size(); k++) {
                std::cout << servers[i].getlocations()[j].get_config().getIndex()[k];
            }
            std::cout << std::endl;
            std::cout << "	" << "Error pages: ";
            for (std::map<uint, std::string>::iterator it = servers[i].getlocations()[j].get_config().getErrorPage().
                         begin();
                 it != servers[i].getlocations()[j].get_config().getErrorPage().end(); it++) {
                std::cout << "	" << it->first << " " << it->second << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Max client body size: " << servers[i].getlocations()[j].get_config().
                    getMaxClientBodySize()
                    << std::endl;
            std::cout << "	" << "Allowed methods: ";
            for (size_t k = 0; k < servers[i].getlocations()[j].get_allowed_methods().size(); k++) {
                std::cout << servers[i].getlocations()[j].get_allowed_methods()[k] << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Autoindex: " << (
                        servers[i].getlocations()[j].get_config().getAutoIndex() ? "on" : "off") <<
                    std::endl;
            std::cout << std::endl;
        }
    }
}

/// @brief Parse the IP address from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void Config::host(const std::string line, server &server) {
    std::string host;
    if (line.find_first_of(".") != std::string::npos) {
        if (line.find_first_of(":") != std::string::npos)
            host = line.substr(line.find_first_of(" ") + 1, line.find_first_of(":") - line.find_first_of(" ") - 1);
        else
            host = line.substr(line.find_first_of(" ") + 1);
    } else {
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
void Config::port(const std::string line, server &server) {
    int port = 80;

    if (line.find_first_of(":") != std::string::npos)
        port = std::atoi(line.substr(line.find_first_of(":") + 1).c_str());
    server.set_port(port);
}

/// @brief Parse the server name from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void Config::server_name(const std::string line, server &server) {
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "server_name")
            server.set_server_name(word);
    }
}

/// @brief Parse the index from the line from the config file and set it to the server object
/// @param line The line from the config file
/// @param server The server object
void Config::index(const std::string line, server &server) {
    server.get_config().getIndex().clear();
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "index")
            server.get_config().setIndex(word);
    }
}

/// @brief Parse location block from the config file and add it to the server object
/// @param line The line from the config file
/// @param server The server object
/// @param line_number The starting line number
/// @param file The config file stream
void Config::location(std::string line, server &server, int &line_number, std::ifstream &file) {
    Location location;
    location.set_path(line.substr(line.find_first_of(" ") + 1, line.find_last_of(" ") - line.find_first_of(" ") - 1));
    while (std::getline(file, line) && !file.eof() && line.find("}") == std::string::npos) {
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
        if (line.find("autoindex") != std::string::npos) {
            std::string autoindex = line.substr(line.find_first_of(" ") + 1);
            location.get_config().setAutoIndex(autoindex == "off" ? false : true);
        }
    }
    server.getlocations().push_back(location);
}

/// @brief Parse the limit_except block from the config file and add it to the location object
/// @param line The line from the config file
/// @param location The location object
void Config::limit_except(std::string line, Location &location) {
    std::stringstream ss(line);
    std::string word;
    while (ss >> word) {
        if (word != "limit_except" && word.find_first_of("{") == std::string::npos)
            location.set_allowed_methods(word);
    }
}