#ifndef SERVER_HPP
# define SERVER_HPP

#include "../server_config/ServerConfig.hpp"
#include "../location/Location.hpp"
#include <vector>

class server {
public:
    server();

    ~server();

    const int &get_port() const;

    const std::string &get_host();

    const vector<std::string> &get_server_name();

    ServerConfig &get_config();

    vector<Location> &getlocations();

    void set_host(const std::string &host);

    void set_port(const int &port);

    void set_server_name(const std::string &server_name);

private:
    std::string host_;
    int port_;
    vector<std::string> server_name_;
    ServerConfig config_;
    vector<Location> locations_;
};

#endif