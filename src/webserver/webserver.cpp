#include <webserver/webserver.hpp>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/epoll.h>

/**
 * @brief Get the servers object
 *
 * @return std::vector<Server>& - Reference to the vector of servers
 */
std::vector<Server> &WebServer::get_servers() { return servers_; }

// Static member variable for logging
std::ofstream WebServer::log_file_;

/**
 * @brief Construct a new WebServer object
 *
 * Opens the log file and logs the start of the WebServer.
 */
WebServer::WebServer() : epoll_fd_(epoll_create(1)) {
    log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!log_file_)
        throw std::runtime_error("Failed to open log file");
    log("WebServer started", info);
}

/**
 * @brief Destroy the WebServer object
 *
 * Closes the log file.
 */
WebServer::~WebServer() {
    log_file_.close();
}

/**
 * @brief Set up the sockets for all servers
 */
void WebServer::setup_sockets() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->socket_setup();
}

/**
 * @brief Set up epoll for handling multiple connections
 *
 * Checks the epoll instance and adds the socket file descriptor of each server to it.
 */
void WebServer::setup_epoll() {
    if (epoll_fd_ == -1)
        log("Failed to create epoll file descriptor", error);
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it) {
        struct epoll_event event;
        event.events = EPOLLIN;
        event.data.fd = it->get_socket_fd();
        if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, it->get_socket_fd(), &event) == -1)
            log("Failed to add file descriptor to epoll", error);
    }
}

/**
 * @brief Handle incoming connections
 *
 * Waits for events on the epoll instance and handles incoming connections.
 */
void WebServer::handle_connections() {
    while (true) {
        struct epoll_event events[10];
        int num_events = epoll_wait(epoll_fd_, events, 10, -1);
        if (num_events == -1)
            log("epoll_wait failed", error);
        for (int i = 0; i < num_events; i++) {
            if (events[i].events & EPOLLIN) {
                sockaddr_in address = {};
                socklen_t addrlen = sizeof(address);
                const int new_socket = accept(events[i].data.fd, reinterpret_cast<struct sockaddr *>(&address),
                                              &addrlen);
                if (new_socket == -1)
                    log("Failed to accept the new client connection", error);
                const char *http_response = HTTP_RESPONSE;
                send(new_socket, http_response, strlen(http_response), 0);
                log("HTTP response sent to the client", info);
            }
        }
    }
}