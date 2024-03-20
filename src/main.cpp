#include <cmath>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include "location/location.hpp"
#include <config.hpp>
#include <webserver/webserver.hpp>

#pragma region test
#include <poll.h>

void test() {
    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address = {};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);
    if (bind(server_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)))
        WebServer::log("Failed to bind the socket", error);
    listen(server_fd, 3);

    pollfd fds[1];
    fds[0].fd = server_fd;
    fds[0].events = POLLIN;
    while (true) {
        int ret = poll(fds, 1, -1);
        if (ret == -1)
            WebServer::log("poll failed", error);
        if (fds[0].revents & POLLIN) {
            socklen_t addrlen = sizeof(address);
            const int new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
            if (new_socket == -1)
                WebServer::log("Failed to accept the new client connection", error);
            const char *http_response = "HTTP/1.1 200 OK\n"
                    "Content-Type: text/html\n"
                    "Content-Length: 600\n\n"
                    "<!DOCTYPE html>\n"
                    "<html>\n"
                    "<head>\n"
                    "    <title>Test Page for Webserver</title>\n"
                    "    <style>\n"
                    "        body {\n"
                    "            background-color: black;\n"
                    "            color: white;\n"
                    "        }\n"
                    "    </style>\n"
                    "</head>\n"
                    "<body>\n"
                    "    <h1>Welcome to Diogo's & Paulo's Webserver</h1>\n"
                    "    <p>This is a test page for our webserver. If you can see this page, it means our webserver is working correctly.</p>\n"
                    "    <a href=https://github.com/diogoforte style=\\\"color: white;\\\">Visit Diogo's GitHub Profile</a>\n"
                    "    <a href=https://github.com/PauloBastos96 style=\\\"color: white;\\\">Visit Paulo's GitHub Profile</a>\n"
                    "</body>\n"
                    "</html>\n";
            send(new_socket, http_response, strlen(http_response), 0);
            WebServer::log("HTTP response sent to the client", info);
        }
    }
}
#pragma endregion

int main(const int ac, const char **av) {
    try {
        WebServer webserver;
        if (ac > 2)
            WebServer::log("webserv: too many arguments", error);
        // Parse the configuration file
        webserver.config_servers(ac == 1 ? "configs/default.conf" : av[1]);
        Config::display_configs(webserver.get_servers());
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
        test();
    } catch (const std::runtime_error &e) {
        return 1;
    } catch (const std::exception &e) {
        std::cerr << RED << "[FATAL]:	" << e.what() << RESET << std::endl;
        return 1;
    }
}