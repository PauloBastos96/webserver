#include "webserver.hpp"
#include <parser/AParserFunctions.hpp>
#include <iostream>


std::vector<Server> &WebServer::getServers() { return servers; }


std::ofstream WebServer::log_file_;

WebServer::WebServer() {
    log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!log_file_)
        throw std::runtime_error("Failed to open log file");
    log("WebServer started", info);
}

WebServer::~WebServer() {
    log_file_.close();
}


void WebServer::parse_config_file(const string &path, vector<Server> &servers) {
    std::ifstream file(path.c_str());
    if (!file.is_open())
        log("[CONFIG] Could not open file " + path, error);
    string line;
    int line_number = 0;
    while (std::getline(file, line) && !file.eof()) {
        log(line, info);
        line_number++;
        if (line.size() >= 2 && line.find("server") != string::npos && line.at(line.size() - 2) == '{') {
            Server server;
            log("test", info);
            while (std::getline(file, line) && !file.eof() && line.find('}') == string::npos) {
                line_number++;
                if (line.find("listen") != string::npos) {
                    AParserFunctions::parseHost(line, server);
                    AParserFunctions::parsePort(line, server);
                }
                if (line.find("server_name") != string::npos)
                    AParserFunctions::parseServerName(line, server);
                if (line.find("root") != string::npos)
                    server.getConfig().setRoot(line.substr(line.find_first_of(' ') + 1));
                if (line.find("index") != string::npos)
                    AParserFunctions::parseIndex(line, server);
                if (line.find("error_page") != string::npos) {
                    if (AParserFunctions::parseErrorPage(line, server))
                        log("[CONFIG] Invalid error page directive", error);
                }
                if (line.find("max_client_body_size") != string::npos)
                    server.getConfig().setMaxClientBodySize(line.substr(line.find_first_of(' ') + 1));
                if (line.find("location") != string::npos && line.at(line.size() - 2) == '{')
                    AParserFunctions::parseLocation(line, server, line_number, file);
                if (line.find("autoindex") != string::npos) {
                    string autoindex = line.substr(line.find_first_of(' ') + 1);
                    server.getConfig().setAutoIndex(autoindex != "off");
                }
            }
            if (line.find('}') == string::npos) {
                log("[CONFIG] Missing closing bracket", error);
            }
            log("[CONFIG] Server configuration parsed", info);
            servers.push_back(server);
        }
    }
    file.close();
}

void WebServer::display_configs(vector<Server> &servers) {
    for (size_t i = 0; i < servers.size(); i++) {
        std::cout << BLUE << "Host " << servers[i].getHost() << ":" << servers[i].getPort() << std::endl;
        std::cout << "Server name: ";
        for (size_t j = 0; j < servers[i].getServerName().size(); j++) {
            std::cout << servers[i].getServerName()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Root: " << servers[i].getConfig().getRoot() << std::endl;
        std::cout << "Index: ";
        for (size_t j = 0; j < servers[i].getConfig().getIndex().size(); j++) {
            std::cout << servers[i].getConfig().getIndex()[j] << " ";
        }
        std::cout << std::endl;
        std::cout << "Error pages: ";
        for (std::map<uint, string>::iterator it = servers[i].getConfig().getErrorPage().begin();
             it != servers[i].getConfig().getErrorPage().end(); it++) {
            std::cout << it->first << "=>" << it->second << " ";
        }
        std::cout << std::endl;
        std::cout << "Max client body size: " << servers[i].getConfig().getMaxClientBodySize() << std::endl;
        std::cout << "Autoindex: " << (servers[i].getConfig().getAutoIndex() ? "on" : "off") << std::endl;
        std::cout << "Locations: " << std::endl;
        for (size_t j = 0; j < servers[i].getLocations().size(); j++) {
            std::cout << "	" << servers[i].getLocations()[j].getPath() << std::endl;
            std::cout << "	" << "Root: " << servers[i].getLocations()[j].getConfig().getRoot() << std::endl;
            std::cout << "	" << "Index: ";
            for (size_t k = 0; k < servers[i].getLocations()[j].getConfig().getIndex().size(); k++) {
                std::cout << servers[i].getLocations()[j].getConfig().getIndex()[k];
            }
            std::cout << std::endl;
            std::cout << "	" << "Error pages: ";
            for (std::map<uint, string>::iterator it = servers[i].getLocations()[j].getConfig().getErrorPage().begin();
                 it != servers[i].getLocations()[j].getConfig().getErrorPage().end(); it++) {
                std::cout << "	" << it->first << " " << it->second << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Max client body size: " << servers[i].getLocations()[j].getConfig().
                    getMaxClientBodySize()
                    << std::endl;
            std::cout << "	" << "Allowed methods: ";
            for (size_t k = 0; k < servers[i].getLocations()[j].getAllowedMethods().size(); k++) {
                std::cout << servers[i].getLocations()[j].getAllowedMethods()[k] << " ";
            }
            std::cout << std::endl;
            std::cout << "	" << "Autoindex: " << (
                        servers[i].getLocations()[j].getConfig().getAutoIndex() ? "on" : "off") <<
                    std::endl;
            std::cout << std::endl;
        }
    }
}