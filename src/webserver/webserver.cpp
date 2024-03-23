#include <webserver/webserver.hpp>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>

std::vector<Server> &WebServer::get_servers() { return servers_; }

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

void WebServer::setup_sockets() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->socket_setup();
}

void WebServer::setup_poll_fds() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it) {
        pollfd pfd = {};
        pfd.fd = it->get_socket_fd();
        pfd.events = POLLIN;
        fds_.push_back(pfd);
    }
}

void WebServer::handle_connections() {
    while (true) {
        const int ret = poll(&fds_[0], fds_.size(), -1);
        if (ret == -1)
            log("poll failed ", error);
        for (std::vector<pollfd>::iterator it = fds_.begin(); it != fds_.end(); ++it) {
            if (it->revents & POLLIN) {
                sockaddr_in address = {};
                socklen_t addrlen = sizeof(address);
                const int new_socket = accept(it->fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
                if (new_socket == -1)
                    log("Failed to accept the new client connection", error);
                const char *http_response = HTTP_RESPONSE;
                send(new_socket, http_response, strlen(http_response), 0);
                log("HTTP response sent to the client", info);
            }
        }
    }
}