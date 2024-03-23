#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <server/server.hpp>
#include "logger.hpp"
#include <vector>

class WebServer {
public:
    WebServer();

    ~WebServer();

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

    // Setup
public:
    void setup_sockets();
    void setup_epoll();
    void handle_connections();
    void setup_servers();

private:
   int epoll_fd_;
};

#endif