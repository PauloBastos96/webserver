#include <cmath>
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
        WebServer::log("test", info);
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
    } catch (std::runtime_error &e) {
        return 1;
    }
    catch (std::exception &e) {
        std::cout << "[ERROR]\t\t" << e.what() << std::endl;
        return 2;
    }
}