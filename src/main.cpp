#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include "webserver/webserver.hpp"
#include "config/config.hpp"
#include "server/Server.hpp"
#include "parser/AParserFunctions.hpp"
#include "webserver/webserver.hpp"

int main(const int ac, const char **av) {
    (void) av;
    try {
        WebServer webserver;
        if (ac > 2)
            WebServer::log("webserv: too many arguments", error);
        // Parse the configuration file
        webserver.parse_config_file(ac == 1 ? "configs/default.conf" : av[1], webserver.getServers());
        webserver.display_configs(webserver.getServers());
        std::cout << webserver.getServers().size();
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