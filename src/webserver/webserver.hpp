#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include "logger.hpp"
#include <server.hpp>
#include <sys/epoll.h>
#include <vector>

#define MAX_EVENTS 10
#define BUFFER_SIZE 10000

class WebServer {
  public:
#pragma region Constructors &Destructors
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
    static bool is_running;

    void setup_server_sockets();

    void setup_epoll();

    void insert_epoll(int socket_fd) const;
#pragma endregion

#pragma region Connection Handling
    void server_routine();

    size_t find_server(int fd);

    void setup_servers();

    void accept_connection(Server &server, int fd) const;

    bool is_server_socket(int fd);

    void handle_connection(Server &server, int fd) const;

    void end_connection(Server &server, int fd) const;

    void parse_http_request(const std::string &data_received);

#pragma endregion

  private:
    std::vector<Server> servers_;
    static std::ofstream log_file_;
    int epoll_fd_;
    epoll_event events_[MAX_EVENTS];
    int status_code_;
};

#endif