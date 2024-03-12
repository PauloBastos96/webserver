#include <iostream>
#include "config.hpp"
#include "../logger/logger.hpp"

Config::Config(const std::string &config_file) {
    file.open(config_file.c_str(), std::ios::in);
    if (!file)
        logger::log("Failed to open Config file", error);
    logger::log("Config file opened", info);
}

Config::~Config() {
    logger::log("Config file closed", info);
    file.close();
}