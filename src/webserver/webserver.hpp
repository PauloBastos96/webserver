#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <server/server.hpp>
#include "logger.hpp"
#include <vector>
#include <sys/epoll.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 10000

class WebServer {
public:
#pragma region Constructors & Destructors
    WebServer();

    ~WebServer();
#pragma endregion

#pragma region Getters
    std::vector<Server> &get_servers();
#pragma endregion

#pragma region Logger
    static void log(const std::string &message, int log_level);
#pragma endregion

#pragma region Setup
    void setup_sockets();

    void setup_epoll();

    void insert_epoll(int socket_fd) const;
#pragma endregion

#pragma region Connection Handling
    void server_routine();

    void setup_servers();

    void accept_connection() const;

    bool is_server_socket();

    void handle_connection() const;

    void end_connection() const;
#pragma endregion

private:
    std::vector<Server> servers_;
    static std::ofstream log_file_;
    int epoll_fd_;
    epoll_event events_[MAX_EVENTS];
    int server_number_;
};

#endif