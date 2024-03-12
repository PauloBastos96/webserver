#ifndef LOCATION_HPP
# define LOCATION_HPP

#include "../server_config/ServerConfig.hpp"

class Location {
private:
    string _path;
    ServerConfig _config;
    vector<string> _allowedMethods;

public:
    Location();

    ~Location();

    const string &getPath(void);

    ServerConfig &getConfig(void);

    vector<string> &getAllowedMethods(void);

    void setPath(const string &path);

    void setAllowedMethods(const string &method);
};

#endif