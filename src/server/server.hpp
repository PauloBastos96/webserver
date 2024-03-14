#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include "config.hpp"

class Location;

class server {
public:
    server();

    ~server();

    const int &get_port() const;

    const std::string &get_host();

    const std::vector<std::string> &get_server_name();

    config &get_config();

    std::vector<Location> &get_locations();

    void set_host(const std::string &host);

    void set_port(const int &port);

    void set_server_name(const std::string &server_name);

private:
    std::string host_;
    int port_;
    std::vector<std::string> server_name_;
    config config_;
    std::vector<Location> locations_;
};

#endif