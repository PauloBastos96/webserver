#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include "config.hpp"

class Location;

class Server {
public:
#pragma region Constructors & Destructors

    Server();

    ~Server();

#pragma endregion

#pragma region Getters

    const int &get_port() const;

    const std::string &get_host();

    const std::vector<std::string> &get_server_name();

    Config &get_config();

    std::vector<Location> &get_locations();

    int &get_socket_fd();

#pragma endregion

#pragma region Setters

    void set_host(const std::string &host);

    void set_port(const int &port);

    void set_server_name(const std::string &server_name);

#pragma endregion

    void socket_setup();

private:
    std::string host_;
    int port_;
    std::vector<std::string> server_name_;
    Config config_;
    std::vector<Location> locations_;
    int socket_fd_;
};

#endif