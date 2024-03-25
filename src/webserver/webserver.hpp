#ifndef WEBSERVER_HPP
#define WEBSERVER_HPP

#include <server.hpp>
#include <sys/epoll.h>
#include <vector>
#include "logger.hpp"

#define MAX_EVENTS 10
#define BUFFER_SIZE 10000


#pragma region Forward Declarations

struct Request {
    std::string method;
    std::string uri;
    std::string http_version;
    std::map<std::string, std::string> headers;
    std::string body;
};

#pragma endregion

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
    static bool is_running;

    void setup_sockets();

    void setup_epoll();

    void insert_epoll(int socket_fd) const;
#pragma endregion

#pragma region Connection Handling
    void server_routine();

    void setup_servers();

    void accept_connection();

    bool is_server_socket();

    void handle_connection();

    void end_connection() const;

    void parse_http_request(const std::string &data_received);

#pragma endregion

private:
    std::vector<Server> servers_;
    static std::ofstream log_file_;
    int epoll_fd_;
    epoll_event events_[MAX_EVENTS];
    int server_number_;
    int status_code_;
    Request request_;
};

#endif