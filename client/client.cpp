#include <arpa/inet.h>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>

int main(int ac, char **av) {
    if (ac != 2) {
        std::cerr << "Usage: " << av[0] << " <number of clients>" << std::endl;
        return 1;
    }
    int clients = std::atoi(av[1]);
    timeval total_start, total_end;
    gettimeofday(&total_start, NULL);
    for (int i = 0; i < clients; i++) {
        int sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock == -1) {
            std::cerr << "Could not create socket" << std::endl;
            return 1;
        }
        std::string server_ip = "127.0.0.1";
        int server_port = 8080;
        sockaddr_in server_addr;
        server_addr.sin_family = AF_INET;
        server_addr.sin_addr.s_addr = inet_addr(server_ip.c_str());
        server_addr.sin_port = htons(server_port);
        if (connect(sock, (struct sockaddr *)&server_addr,
                    sizeof(server_addr)) < 0) {
            std::cerr << "Connect failed" << std::endl;
            return 1;
        }
        std::string message = "Hello, server!";
        if (send(sock, message.c_str(), message.size(), 0) < 0) {
            std::cerr << "Send failed" << std::endl;
            return 1;
        }
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
            std::cerr << "Receive failed" << std::endl;
            return 1;
        }
        std::cout << "Server response: " << buffer << std::endl;
        close(sock);
    }
    gettimeofday(&total_end, NULL);
    double total_elapsed =
        (total_end.tv_sec - total_start.tv_sec) +
        (total_end.tv_usec - total_start.tv_usec) / 1000000.0;
    std::cout << "Total time for all requests: " << total_elapsed << " s"
              << std::endl;
    return 0;
}