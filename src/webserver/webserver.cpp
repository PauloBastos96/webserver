#include <webserver/webserver.hpp>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/epoll.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>

std::ofstream WebServer::log_file_;

#pragma region Constructors & Destructors

WebServer::WebServer() : epoll_fd_(-1), events_(), server_number_(0) {
    log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!log_file_)
        throw std::runtime_error("Failed to open log file");
    log("WebServer started", info);
}

WebServer::~WebServer() {
    log_file_.close();
    if (epoll_fd_ > 0)
        close(epoll_fd_);
}

#pragma endregion

#pragma region Getters

std::vector<Server> &WebServer::get_servers() { return servers_; }

#pragma endregion

#pragma region Setup

void WebServer::setup_sockets() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->socket_setup();
}

void WebServer::setup_epoll() {
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1)
        log("Failed to create epoll file descriptor", error);
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        insert_epoll(it->get_socket_fd());
}

void WebServer::insert_epoll(int socket_fd) const {
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd, &event) == -1)
        log("Failed to add file descriptor to epoll", error);
}

#pragma endregion

#pragma region Connection Handling

bool WebServer::is_server_socket() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        if (it->get_socket_fd() == events_[server_number_].data.fd)
            return true;
    return false;
}

void WebServer::accept_connection() const {
    sockaddr_in address = {};
    socklen_t addrlen = sizeof(address);
    const int client_socket = accept(events_[server_number_].data.fd, reinterpret_cast<struct sockaddr *>(&address),
                                     &addrlen);
    if (client_socket == -1)
        log("Failed to accept the new client connection", warning);
    const int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        log("Failed to set server socket to non-blocking mode", warning);
    insert_epoll(client_socket);
    log("Connection accepted", info);
}

void WebServer::handle_connection() const {
    char buffer[BUFFER_SIZE];
    const ssize_t bytes_read = recv(events_[server_number_].data.fd, buffer, sizeof(buffer), 0);
    if (bytes_read > 0) {
        const char *http_response = HTTP_RESPONSE;
        if (send(events_[server_number_].data.fd, http_response, strlen(http_response), 0) == -1)
            log("Failed to send the HTTP response", warning);
        else
            log("HTTP response sent to the client", info);
    } else {
        end_connection();
        if (!bytes_read)
            log("Client disconnected", info);
        else
            log("Failed to read from the client", warning);
    }
}

void WebServer::end_connection() const {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, events_[server_number_].data.fd, NULL) == -1)
        log("Failed to remove file descriptor from epoll", warning);
    close(events_[server_number_].data.fd);
}

void WebServer::server_routine() {
    while (true) {
        const int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
        if (num_events == -1)
            log("epoll failed", error);
        for (server_number_ = 0; server_number_ < num_events; server_number_++) {
            if (events_[server_number_].events & EPOLLIN) {
                if (is_server_socket())
                    accept_connection();
                else
                    handle_connection();
            }
        }
    }
}

#pragma endregion