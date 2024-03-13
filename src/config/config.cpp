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
                    AParserFunctions::host(line, server);
                    AParserFunctions::port(line, server);
                }
                if (line.find("server_name") != std::string::npos)
                    AParserFunctions::server_name(line, server);
                if (line.find("root") != std::string::npos)
                    server.get_config().setRoot(line.substr(line.find_first_of(' ') + 1));
                if (line.find("index") != std::string::npos)
                    AParserFunctions::index(line, server);
                if (line.find("error_page") != std::string::npos) {
                    if (AParserFunctions::parseErrorPage(line, server))
                        WebServer::log("[CONFIG] Invalid error page directive", error);
                }
                if (line.find("max_client_body_size") != std::string::npos)
                    server.get_config().setMaxClientBodySize(line.substr(line.find_first_of(' ') + 1));
                if (line.find("location") != std::string::npos && line.at(line.size() - 2) == '{')
                    AParserFunctions::location(line, server, line_number, file);
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
            for (std::map<uint, std::string>::iterator it = servers[i].getlocations()[j].get_config().getErrorPage().begin();
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