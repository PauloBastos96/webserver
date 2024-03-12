#include <config.h>
#include <iostream>
#include <logger.h>

Config::Config(const std::string &config_file) {
    file.open(config_file.c_str(), std::ios::in);
    if (!file)
        throw std::runtime_error("Failed to open log file");
    logger::log("Config file opened", info);
}

Config::~Config() {
    logger::log("Config file closed", info);
    file.close();
}