#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <fstream>
#include <vector>
#include <server/Server.hpp>

class Config {
public:
    explicit Config(const std::string &config_file);

    ~Config();

    static void parse_config_file(const std::string &path, std::vector<server> &servers);

    static void display_configs(vector<server> &servers);
private :
    std::ofstream file;
};

#endif