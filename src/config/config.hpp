#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <string>
#include <fstream>

class Config {
public:
    explicit Config(const std::string &config_file);

    ~Config();

private :
    std::ofstream file;
};

#endif