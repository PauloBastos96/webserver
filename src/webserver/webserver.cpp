#include "webserver.hpp"
#include <config/config.hpp>
#include <iostream>
#include <config.hpp>


std::vector<server> &WebServer::get_servers() { return servers; }

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

void WebServer::config_servers(const std::string &path, std::vector<server> &servers) {
    config::parse_config_file(path, servers);
}