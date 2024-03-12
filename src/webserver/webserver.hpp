#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "../server/Server.hpp"
#include "logger.hpp"
#include <vector>

class WebServer {
public:
    WebServer();

    ~WebServer();
// Server
public:
    std::vector<Server> &getServers();
private:
    std::vector<Server> servers;
// Logger
public:
    static void log(const std::string &message, int log_level);
private:
    static std::ofstream log_file_;
// Parser
public:
    static void parse_config_file(const std::string &path, std::vector<Server> &servers);
    static void display_configs(vector<Server> &servers);
};

#endif