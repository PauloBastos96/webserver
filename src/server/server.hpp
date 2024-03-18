#ifndef SERVER_HPP
# define SERVER_HPP

#include <vector>
#include "config.hpp"

class Location;

class Server {
public:
    Server();

    ~Server();

    const int &get_port() const;

    const std::string &get_host();

    const std::vector<std::string> &get_server_name();

    config &get_config();

    std::vector<Location> &get_locations();

    void set_host(const std::string &host);

    void set_port(const int &port);

    void set_server_name(const std::string &server_name);

    void start(void);

    void stop(void);

private:
    std::vector<std::string> server_name_;
    std::vector<Location> locations_;
    std::string host_;
    config config_;
    int socket_fd_;
    int port_;
};

#endif