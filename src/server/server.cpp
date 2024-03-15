#include <vector>
#include <server.hpp>
#include <location.hpp>

server::server() {
    port_ = 80;
    host_ = "127.0.0.1";
    config_.set_root("/");
    config_.set_max_client_body_size("1m");
    config_.set_index("index.html");
    config_.set_auto_index(false);
}

server::~server() {
};

void server::set_host(const std::string &host) {
    host_ = host;
}

void server::set_port(const int &port) {
    port_ = port;
}

void server::set_server_name(const std::string &server_name) {
    server_name_.push_back(server_name);
}

void config::set_root(const std::string &root) {
    root_ = root;
}

const int &server::get_port() const {
    return (port_);
}

const std::string &server::get_host() {
    return (host_);
}

const std::vector<std::string> &server::get_server_name() {
    return (server_name_);
}

config &server::get_config() {
    return (config_);
}

std::vector<Location> &server::get_locations() {
    return (locations_);
}