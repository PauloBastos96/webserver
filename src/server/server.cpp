#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <poll.h>
#include <iostream>
#include <fstream>
#include "server.hpp"
#include "location.hpp"
#include "logger.hpp"
#include "webserver.hpp"

Server::Server() {
    port_ = 80;
    host_ = "127.0.0.1";
    config_.set_root("/");
    config_.set_max_client_body_size("1m");
    config_.set_index("index.html");
    config_.set_auto_index(false);
    socket_fd_ = -1;
}

Server::~Server() {
    if (socket_fd_ != -1)
        close(socket_fd_);
};

void Server::set_host(const std::string &host) {
    host_ = host;
}

void Server::set_port(const int &port) {
    port_ = port;
}

void Server::set_server_name(const std::string &server_name) {
    server_name_.push_back(server_name);
}

void config::set_root(const std::string &root) {
    root_ = root;
}

const int &Server::get_port() const {
    return (port_);
}

const std::string &Server::get_host() {
    return (host_);
}

const std::vector<std::string> &Server::get_server_name() {
    return (server_name_);
}

config &Server::get_config() {
    return (config_);
}

std::vector<Location> &Server::get_locations() {
    return (locations_);
}

void Server::start(void){
    struct addrinfo *result;
    struct addrinfo hints;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    std::stringstream ss;
    ss << port_;
    int s = getaddrinfo(host_.c_str(), ss.str().c_str(), &hints, &result);
    if (s != 0)
        WebServer::log("getaddrinfo failed", error);
    socket_fd_ = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (socket_fd_ == -1)
        WebServer::log(std::string(ERR_SOCKET) + host_ + ":" + ss.str() , error);
    int opt = 1;
    setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
    if (bind(socket_fd_, result->ai_addr, result->ai_addrlen) == -1)
    {
        close(socket_fd_);
        WebServer::log(std::string(ERR_BIND) + host_ + ":" + ss.str(), error);
    }
    listen(socket_fd_, 10);
    struct sockaddr_in client;
    socklen_t client_size = sizeof(client);
    char buffer[1024];
    int client_fd;
    client_fd = accept(socket_fd_, (struct sockaddr *)&client, &client_size);
    if (client_fd == -1)
    {
        close(socket_fd_);
        WebServer::log("accept failed", error);
    }
    pollfd fds[1];
    fds[0].fd = client_fd;
    fds[0].events = POLLIN | POLLOUT;
    while (true)
    {
        int ret = poll(fds, 1, 1000);
        int bytes = 0;
        if (ret == -1)
        {
            close(client_fd);
            close(socket_fd_);
            WebServer::log("poll failed", error);
        }
        if (fds[0].revents & POLLIN)
        {
            bytes = read(client_fd, buffer, 1024);
            if (bytes == -1)
                WebServer::log("read failed", error);
            if (bytes == 0)
                continue;
            buffer[bytes] = 0;
            WebServer::log(buffer, debug);
            ret = poll(fds, 1, 1000);
            if (ret == -1)
            {
                close(client_fd);
                close(socket_fd_);
                WebServer::log("poll failed", error);
            }
            if (fds[0].revents & POLLOUT)
			{
                std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
                write(client_fd, response.c_str(), response.length());
                std::fstream file;
                file.open("test_pages/index.html", std::ios::in);
                if (file.is_open())
                {
                    while (file.getline(buffer, 1024))
                    {
                        write(client_fd, buffer, strlen(buffer));
                    }
                    file.close();
                }
                close(client_fd);
                close(socket_fd_);
                socket_fd_ = -1;
				break;
			}
        }
    }
    freeaddrinfo(result);
}