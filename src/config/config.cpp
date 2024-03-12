#include <iostream>
#include "config.hpp"
#include "../webserver/webserver.hpp"

Config::Config(const std::string &config_file) {
    file.open(config_file.c_str(), std::ios::in);
    if (!file)
        WebServer::log("Failed to open Config file", error);
    WebServer::log("Config file opened", info);
}

Config::~Config() {
    WebServer::log("Config file closed", info);
    file.close();
}