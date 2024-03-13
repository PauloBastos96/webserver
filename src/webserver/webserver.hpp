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
    std::vector<server> &get_servers();
private:
    std::vector<server> servers;
// Logger
public:
    static void log(const std::string &message, int log_level);
private:
    static std::ofstream log_file_;
// Parser
public:
    class parser;
};

#endif