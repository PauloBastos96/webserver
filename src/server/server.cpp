#include <vector>
#include <server/server.hpp>
#include "location/location.hpp"

#pragma region Constructors & Destructors

Server::Server() {
    port_ = 80;
    host_ = "127.0.0.1";
    config_.set_root("/");
    config_.set_max_client_body_size("1m");
    config_.set_index("index.html");
    config_.set_auto_index(false);
}

Server::~Server() {
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

#pragma endregion