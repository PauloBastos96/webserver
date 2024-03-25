#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <string>
#include <config.hpp>
#include <location.hpp>

class Location;
class Config;

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

    const int &get_socket_fd() const;

    std::vector<int> &get_connected_clients();

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
    std::vector<int> connected_clients_;
};

#endif