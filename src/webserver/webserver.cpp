#include <algorithm>
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

WebServer::WebServer() : epoll_fd_(-1), events_() {
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
        const int option_value = 1;
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_value,
                       sizeof(option_value)) == -1)
            log("Failed to set server socket options", warning);
        const int flags = fcntl(server_socket, F_GETFL, 0);
        if (flags == -1 ||
            fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1)
            log("Failed to set server socket to non-blocking mode", warning);
        sockaddr_in address = {};
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(it->get_port());
        if (bind(server_socket, reinterpret_cast<sockaddr *>(&address),
                 sizeof(address)))
            log("Failed to bind the server socket", warning);
        if (listen(server_socket, 10))
            log("Failed to listen on the server socket", warning);
        it->set_socket(server_socket);
        insert_epoll(it->get_socket());
        std::stringstream ss;
        ss << it->get_socket();
        log("Server (" + ss.str() + ") is listening", info);
    }
}

void WebServer::insert_epoll(const int socket) const {
    epoll_event event = {};
    event.events = EPOLLIN | EPOLLOUT;
    event.data.fd = socket;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket, &event) == -1)
        log("Failed to add file descriptor to epoll", error);
}

#pragma endregion

#pragma region Utils

/// @brief Check if the socket is a server socket
/// @param socket The socket to check
/// @return True if the socket is a server socket, false if it's a client socket
bool WebServer::is_server(int socket) {
    for (std::vector<Server>::iterator server_it = servers_.begin();
         server_it != servers_.end(); ++server_it) {
        if (server_it->get_socket() == socket)
            return true;
    }
    return false;
}

/// @brief Find the server that the socket fd belongs to
/// @param fd The client socket
/// @return The server that's connected to the client
std::vector<Server>::iterator WebServer::find_server(const int socket) {
    for (std::vector<Server>::iterator server_it = servers_.begin();
         server_it != servers_.end(); ++server_it) {
        if (server_it->get_socket() == socket)
            return server_it;
        for (std::vector<int>::iterator client_it =
                 server_it->get_connected_clients().begin();
             client_it != server_it->get_connected_clients().end(); ++client_it)
            if (*client_it == socket)
                return server_it;
    }
    return servers_.end();
}

#pragma endregion

#pragma region Connection Handling

/// @brief Send the response to the client
/// @param socket The client socket
/// @param response The response to send
void WebServer::send_response(const int socket, std::string &response) const {
    if (response.empty())
        return;
    size_t bytes_sent = 0;
    while (bytes_sent < response.length()) {
        ssize_t sent = send(socket, response.c_str() + bytes_sent,
                            response.length() - bytes_sent, 0);
        if (sent == -1)
            WebServer::log("Failed to send response to the client", error);
        bytes_sent += sent;
    }
    response.clear();
}

/// @brief Accept a new client connection
/// @param server The server that the client is connecting to
/// @param socket The client socket
void WebServer::accept_connection(Server &server, const int socket) const {
    sockaddr_in address = {};
    socklen_t address_length = sizeof(address);
    const int client_socket = accept(
        socket, reinterpret_cast<struct sockaddr *>(&address), &address_length);
    if (client_socket == -1)
        log("Failed to accept the client connection", error);
    const int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        log("Failed to set client socket to non-blocking mode", error);
    server.get_connected_clients().push_back(client_socket);
    if (client_socket != -1)
        insert_epoll(client_socket);
    log("Connection accepted", info);
}

/// @brief Handle the client connection
/// @param server The server that the client is connected to
/// @param socket The client socket
/// @return The response to send to the client
std::string WebServer::handle_connection(Server &server,
                                         const int socket) const {
    std::string response;
    char buffer[BUFFER_SIZE];
    const ssize_t bytes_received = recv(socket, buffer, BUFFER_SIZE, 0);
    if (!bytes_received || bytes_received == -1) {
        end_connection(server, socket);
        log(!bytes_received ? "Client disconnected"
                            : "Failed to receive data from the client",
            info);
        return response;
    }

    const std::string data_received(buffer);
    HttpHandler http_handler(data_received, server);
    return http_handler.process_request();
}

/// @brief Close the connection with the client
/// @param server The server that the client is connected to
/// @param socket The client socket to close
void WebServer::end_connection(Server &server, int socket) const {
    std::stringstream ss;
    ss << socket;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, socket, NULL) == -1)
        log("Failed to remove socket " + ss.str() + " from the epoll", error);
    close(socket);
    const std::vector<int>::iterator client_it =
        std::find(server.get_connected_clients().begin(),
                  server.get_connected_clients().end(), socket);
    if (client_it != server.get_connected_clients().end())
        server.get_connected_clients().erase(client_it);
}

/// @brief The main server loop
void WebServer::server_routine() {
    setup_server_sockets();
    std::string response;
    while (is_running) {
        try {
            const int num_events =
                epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
            if (num_events == -1)
                log("Failed to wait on epoll", error);
            for (int i = 0; i < num_events; i++) {
                const int socket = events_[i].data.fd;
                if (events_[i].events & EPOLLIN) {
                    const std::vector<Server>::iterator server_it =
                        find_server(socket);
                    if (server_it == servers_.end()) {
                        log("Failed to find server", error);
                        continue;
                    }
                    if (is_server(socket))
                        accept_connection(*server_it, socket);
                    else
                        response = handle_connection(*server_it, socket);
                }
                if (events_[i].events & EPOLLOUT)
                    send_response(socket, response);
                if (events_[i].events & EPOLLERR ||
                    events_[i].events & EPOLLHUP) {
                    const std::vector<Server>::iterator server_it =
                        find_server(socket);
                    log("EPOLLERR || EPOLLUP", warning);
                    (server_it != servers_.end())
                        ? end_connection(*server_it, socket)
                        : log("Client already removed", error);
                }
            }
        } catch (const std::exception &e) {
            if (!strstr(e.what(), "[ERROR]"))
                log(e.what(), debug);
        }
    }
}

#pragma endregion