#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <http_handler.hpp>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <webserver.hpp>

std::ofstream WebServer::log_file_;

bool WebServer::is_running = true;

#pragma region Constructors &Destructors

WebServer::WebServer() : epoll_fd_(-1), events_(), status_code_() {
    log_file_.open("logs/webserv.log", std::ios::out | std::ios::trunc);
    if (!log_file_)
        throw std::runtime_error("Failed to open log file");
    log("WebServer started", info);
}

WebServer::~WebServer() {
    log_file_.close();
    if (epoll_fd_ > 0)
        close(epoll_fd_);
}

#pragma endregion

#pragma region Getters

std::vector<Server> &WebServer::get_servers() { return servers_; }

#pragma endregion

#pragma region Setup

void WebServer::setup_server_sockets() {
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1)
        log("Failed to create epoll file descriptor", warning);
    for (std::vector<Server>::iterator it = servers_.begin();
         it != servers_.end(); ++it) {
        const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
        if (server_socket == -1)
            log("Failed to create the server socket", warning);
        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(it->get_port());
        if (bind(server_socket, reinterpret_cast<sockaddr *>(&address),
                 sizeof(address)))
            log("Failed to bind the server socket", warning);
        const int optval = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &optval,
                       sizeof(optval)) == -1)
            log("Failed to set server socket options", warning);
        const int flags = fcntl(server_socket, F_GETFL, 0);
        if (flags == -1 ||
            fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1)
            log("Failed to set server socket to non-blocking mode", warning);
        if (listen(server_socket, 10))
            log("Failed to listen on the server socket", warning);
        it->set_socket(server_socket);
        insert_epoll(server_socket);
    }
}

void WebServer::insert_epoll(const int socket_fd) const {
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd, &event) == -1)
        log("Failed to add file descriptor to epoll", warning);
}

#pragma endregion

#pragma region Connection Handling

size_t WebServer::find_server(const int fd) {
    for (size_t i = 0; i < servers_.size(); ++i)
        for (std::vector<int>::iterator client_it =
                 servers_[i].get_connected_clients().begin();
             client_it != servers_[i].get_connected_clients().end();
             ++client_it)
            if (*client_it == fd)
                return i;
    return -1;
}

void WebServer::accept_connection(Server &server, const int fd) {
    sockaddr_in address = {};
    socklen_t addrlen = sizeof(address);
    const int client_socket =
        accept(fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
    if (client_socket == -1)
        log("Failed to accept the client connection", warning);
    const int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        log("Failed to set client socket to non-blocking mode", warning);
    server.get_connected_clients().push_back(client_socket);
    insert_epoll(client_socket);
    log("Connection accepted", info);
}

void WebServer::handle_connection(Server &server, const int fd) {
    char buffer[BUFFER_SIZE];
    const ssize_t bytes_received = recv(fd, buffer, BUFFER_SIZE, 0);
    if (!bytes_received || bytes_received == -1) {
        end_connection(server, fd);
        log(!bytes_received ? "Client disconnected"
                            : "Failed to receive data from the client",
            info);
        return;
    }
    const std::string data_received(buffer);
    HttpHandler http_handler(data_received, fd, server);
    http_handler.processRequest();
    // std::string response = "HTTP/1.1 200 OK\r\n"
    //     "Content-Type: text/plain\r\n"
    //     "Content-Length: 12\r\n\r\n"
    //     "Hello, World!";
    // if (send(fd, response.c_str(), response.length(), 0) == -1) {
    //     log("Failed to send test HTTP response", warning);
    // }
    // log("Connection handled", info);
}

void WebServer::end_connection(Server &server, int fd) const {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
        log("Failed to remove file descriptor from epoll", warning);
    close(fd);
    std::vector<int>::iterator it = server.get_connected_clients().begin();
    for (; it != server.get_connected_clients().end(); ++it)
        if (*it == fd)
            break;
    server.get_connected_clients().erase(it);
}

void WebServer::server_routine() {
    setup_server_sockets();
    while (is_running) {
        const int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
        if (num_events == -1)
            log("epoll failed", warning);
        for (int i = 0; i < num_events; i++) {
            if (events_[i].events & EPOLLIN) {
                const int fd = events_[i].data.fd;
                const int server_number = static_cast<int>(find_server(fd));
                server_number == -1
                    ? accept_connection(servers_[i], fd)
                    : handle_connection(servers_[server_number], fd);
            }
        }
    }
}

#pragma endregion