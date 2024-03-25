#include <csignal>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#include <webserver/webserver.hpp>

std::ofstream WebServer::log_file_;

#pragma region Constructors & Destructors

WebServer::WebServer() : epoll_fd_(-1), events_(), server_number_(0), status_code_(){
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

void WebServer::setup_sockets() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        it->socket_setup();
}

void WebServer::setup_epoll() {
    epoll_fd_ = epoll_create(1);
    if (epoll_fd_ == -1)
        log("Failed to create epoll file descriptor", error);
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        insert_epoll(it->get_socket_fd());
}

void WebServer::insert_epoll(int socket_fd) const {
    epoll_event event = {};
    event.events = EPOLLIN;
    event.data.fd = socket_fd;
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd, &event) == -1)
        log("Failed to add file descriptor to epoll", error);
}

#pragma endregion

#pragma region Connection Handling

bool WebServer::is_server_socket() {
    for (std::vector<Server>::iterator it = servers_.begin(); it != servers_.end(); ++it)
        if (it->get_socket_fd() == events_[server_number_].data.fd)
            return true;
    return false;
}

void WebServer::accept_connection() const {
    sockaddr_in address = {};
    socklen_t addrlen = sizeof(address);
    const int client_socket =
            accept(events_[server_number_].data.fd, reinterpret_cast<struct sockaddr *>(&address), &addrlen);
    if (client_socket == -1)
        log("Failed to accept the new client connection", warning);
    const int flags = fcntl(client_socket, F_GETFL, 0);
    if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        log("Failed to set server socket to non-blocking mode", warning);
    insert_epoll(client_socket);
    log("Connection accepted", info);
}

void WebServer::parse_http_request(const std::string &data_received) {
    request_ = Request();
    std::string::size_type pos = data_received.find("\r\n\r\n");
    std::string header_part = data_received.substr(0, pos);
    request_.body = pos != std::string::npos ? data_received.substr(pos + 4) : "";

    std::istringstream request_stream(header_part);
    std::getline(request_stream, request_.method, ' ');
    std::getline(request_stream, request_.uri, ' ');
    std::getline(request_stream, request_.http_version);

    std::string header_line;
    while (std::getline(request_stream, header_line) && header_line != "\r") {
        std::istringstream header_line_stream(header_line);
        std::string header_name;
        std::getline(header_line_stream, header_name, ':');
        std::string header_value;
        std::getline(header_line_stream, header_value);
        request_.headers[header_name] = header_value;
    }

    log("Method: " + request_.method, info);
    log("URI: " + request_.uri, info);
    log("HTTP Version: " + request_.http_version, info);
    log("Headers: ", info);
    for (std::map<std::string, std::string>::const_iterator it = request_.headers.begin(); it != request_.headers.end(); ++it) {
        log(it->first + ": " + it->second, info);
    }
    log("Body: " + request_.body, info);
    log("\n\n", info);
}

void WebServer::handle_connection() {
    char buffer[BUFFER_SIZE];
    const ssize_t bytes_received = recv(events_[server_number_].data.fd, buffer, BUFFER_SIZE, 0);
    if (!bytes_received || bytes_received == -1) {
        end_connection();
        log(!bytes_received ? "Client disconnected" : "Failed to receive data from the client", info);
        return;
    }
    const std::string data_received(buffer);
    parse_http_request(data_received);

    const std::string http_response = HTTP_RESPONSE;
    log((send(events_[server_number_].data.fd, http_response.c_str(), http_response.size(), 0) != -1)
                ? "HTTP response sent to the client"
                : "Failed to send the HTTP response",
        info);
}

void WebServer::end_connection() const {
    if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, events_[server_number_].data.fd, NULL) == -1)
        log("Failed to remove file descriptor from epoll", warning);
    close(events_[server_number_].data.fd);
}

void WebServer::server_routine() {
    while (true) {
        const int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
        if (num_events == -1)
            log("epoll failed", error);
        for (server_number_ = 0; server_number_ < num_events; server_number_++) {
            if (events_[server_number_].events & EPOLLIN) {
                is_server_socket() ? accept_connection() : handle_connection();
            }
        }
    }
}

#pragma endregion