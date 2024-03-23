#include <unistd.h>
#include <vector>
#include <webserver.hpp>
#include <sys/socket.h>
#include <netinet/in.h>
#include <server/server.hpp>
#include "location/location.hpp"

#pragma region Constructors & Destructors

Server::Server() {
    port_ = 8080;
    host_ = "127.0.0.1";
    config_.set_root("/");
    config_.set_max_client_body_size("1m");
    config_.set_index("index.html");
    config_.set_auto_index(false);
    socket_fd_ = -1;
}

Server::~Server() {
    close(socket_fd_);
};

#pragma endregion

#pragma region Setters

void Server::set_host(const std::string &host) {
    host_ = host;
}

void Server::set_port(const int &port) {
    port_ = port;
}

void Server::set_server_name(const std::string &server_name) {
    server_name_.push_back(server_name);
}

#pragma endregion

#pragma region Getters

const int &Server::get_port() const {
    return (port_);
}

const std::string &Server::get_host() {
    return (host_);
}

const std::vector<std::string> &Server::get_server_name() {
    return (server_name_);
}

Config &Server::get_config() {
    return (config_);
}

std::vector<Location> &Server::get_locations() {
    return (locations_);
}

int &Server::get_socket_fd() {
    return (socket_fd_);
}

#pragma endregion

#pragma region Socket

void Server::socket_setup() {
    socket_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port_);
    if (bind(socket_fd_, reinterpret_cast<sockaddr *>(&address), sizeof(address)))
        WebServer::log("Failed to bind the socket", error);
    listen(socket_fd_, 3);
}

#pragma endregion