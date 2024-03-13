#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "../server_config/ServerConfig.hpp"
#include <string>

class Location {
private:
    std::string path_;
    ServerConfig config_;
    vector<std::string> allowed_methods_;

public:
    Location();

    ~Location();

    const std::string &get_path();

    ServerConfig &get_config();

    vector<std::string> &get_allowed_methods();

    void set_path(const std::string &path);

    void set_allowed_methods(const std::string &method);
};

#endif