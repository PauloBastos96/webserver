#ifndef CONFIG_H
#define CONFIG_H

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