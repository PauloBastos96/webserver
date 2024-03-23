#include <cmath>
#include <iostream>
#include <vector>
#include "location/location.hpp"
#include <config.hpp>
#include <webserver/webserver.hpp>

int main(const int ac, const char **av) {
    try {
        WebServer webserver;
        if (ac > 2)
            WebServer::log("webserv: too many arguments", error);
        Config::parse_config_file(ac == 1 ? "configs/default.conf" : av[1], webserver.get_servers());
        Config::display_configs(webserver.get_servers());
        webserver.setup_sockets();
        webserver.setup_poll_fds();
        webserver.handle_connections();
    } catch (const std::runtime_error &e) {
        return 1;
    } catch (const std::exception &e) {
        std::cerr << RED << "[FATAL]	" << e.what() << RESET << std::endl;
        return 1;
    }
}