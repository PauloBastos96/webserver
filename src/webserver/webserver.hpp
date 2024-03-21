#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "server.hpp"
#include "logger.hpp"
#include <vector>

class WebServer {
public:
    WebServer();

    ~WebServer();
    
    static bool is_running;
    // Servers
public:
    std::vector<Server> &get_servers();

private:
    std::vector<Server> servers_;

    // Logger
public:
    static void log(const std::string &message, int log_level);

private:
    static std::ofstream log_file_;

    // Config
public:
    void config_servers(const std::string &path);
};

#endif