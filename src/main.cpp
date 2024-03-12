#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <logger/logger.h>

Logger logger;

int main() {
    try {
        const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(8080);
        if (bind(server_fd, reinterpret_cast<sockaddr *>(&address), sizeof(address)))
            logger.log("Failed to bind the socket", ERROR);
        listen(server_fd, 3);
        socklen_t addrlen = sizeof(address);
        while (true) {
            const int new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
            const char *http_response = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";
            send(new_socket, http_response, strlen(http_response), 0);
            logger.log("HTTP response sent to the client\n", INFO);
        }
    } catch (...) {
        return 1;
    }
}