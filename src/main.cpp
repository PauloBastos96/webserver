#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <logger.h>
#include <config.h>

int main(const int ac, const char **av) {
    (void) av;
    try {
        logger logger;
        if (ac > 2)
            logger::log("webserv: too many arguments", error);
        // Parse the configuration file
        Config config(ac == 1 ? "configs/default.conf" : av[1]);
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