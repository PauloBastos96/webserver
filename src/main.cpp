#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <location.hpp>
#include <server.hpp>
#include <config.hpp>
#include <webserver.hpp>

int main(const int ac, const char **av) {
    try {
        WebServer webserver;
        if (ac > 2)
            WebServer::log("webserv: too many arguments", error);
        // Parse the configuration file
        WebServer::config_servers(ac == 1 ? "configs/default.conf" : av[1], webserver.get_servers());
        config::display_configs(webserver.get_servers());
        // Setup the server
        // ...
        // while (true) {
        // Use poll() to handle multiple client connections
        // ...

        // For each client connection
        // ...

        // Read the HTTP request
        // ...

        // Process the request
        // ...

        // Generate the HTTP response
        // ...

        // Send the response
        // ...

        // End of each client connection
        // }
    } catch (...) {
        return 1;
    }
}