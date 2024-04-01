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
    while (it->get_socket() == -1) {
      const int server_socket = socket(AF_INET, SOCK_STREAM, 0);
      if (server_socket == -1)
        log("Failed to create the server socket", warning);
      sockaddr_in address = {};
      address.sin_family = AF_INET;
      address.sin_addr.s_addr = INADDR_ANY;
      address.sin_port = htons(it->get_port());
      const int option_value = 1;
      if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &option_value,
                     sizeof(option_value)) == -1)
        log("Failed to set server socket options", warning);
      const int flags = fcntl(server_socket, F_GETFL, 0);
      if (flags == -1 ||
          fcntl(server_socket, F_SETFL, flags | O_NONBLOCK) == -1)
        log("Failed to set server socket to non-blocking mode", warning);
      if (bind(server_socket, reinterpret_cast<sockaddr *>(&address),
               sizeof(address)))
        log("Failed to bind the server socket", warning);
      if (listen(server_socket, 10))
        log("Failed to listen on the server socket", warning);
      it->set_socket(server_socket);
    }
    insert_epoll(it->get_socket());
    std::stringstream ss;
    ss << it->get_socket();
    log("Server (" + ss.str() + ") is listening", info);
  }
}

void WebServer::insert_epoll(const int socket_fd) const {
  epoll_event event = {};
  event.events = EPOLLIN;
  event.data.fd = socket_fd;
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, socket_fd, &event) == -1)
    log("Failed to add file descriptor to epoll", error);
}

#pragma endregion

#pragma region Connection Handling

bool WebServer::is_server(int fd) {
  for (size_t i = 0; i < servers_.size(); i++)
    if (servers_[i].get_socket() == fd)
      return true;
  return false;
}

std::vector<Server>::iterator WebServer::find_server(const int fd) {
  for (std::vector<Server>::iterator server_it = servers_.begin();
       server_it != servers_.end(); ++server_it) {
    if (server_it->get_socket() == fd)
      return server_it;
    for (std::vector<int>::iterator client_it =
             server_it->get_connected_clients().begin();
         client_it != server_it->get_connected_clients().end(); ++client_it)
      if (*client_it == fd)
        return server_it;
  }
  return servers_.end();
}

void WebServer::accept_connection(Server &server, const int fd) const {
  sockaddr_in address = {};
  socklen_t address_length = sizeof(address);
  std::cout << "fd = " << fd << std::endl;
  std::cout << "server socket = " << server.get_socket() << std::endl;
  const int client_socket = accept(
      fd, reinterpret_cast<struct sockaddr *>(&address), &address_length);
  if (client_socket == -1)
    log("Failed to accept the client connection", error);
  const int flags = fcntl(client_socket, F_GETFL, 0);
  if (flags == -1 || fcntl(client_socket, F_SETFL, flags | O_NONBLOCK) == -1)
    log("Failed to set client socket to non-blocking mode", error);
  server.get_connected_clients().push_back(client_socket);
  if (client_socket != -1)
    insert_epoll(client_socket);
  std::cout << "Added client socket " << client_socket << " to epoll"
            << std::endl;
  log("Connection accepted", info);
}

void WebServer::handle_connection(Server &server, const int fd) const {
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
  http_handler.process_request();
  //    std::cout << server.get_port() << std::endl;
  // std::string response = (server.get_port() == 8080)
  //                            ? "HTTP/1.1 200 OK\r\n"
  //                              "Content-Type: text/plain\r\n"
  //                              "Content-Length: 12\r\n\r\n"
  //                              "Hello, World!"
  //                            : "HTTP/1.1 200 OK\r\n"
  //                              "Content-Type: text/plain\r\n"
  //                              "Content-Length: 12\r\n\r\n"
  //                              "World, Hello!";
  // if (send(fd, response.c_str(), response.length(), 0) == -1)
  //     log("Failed to send test HTTP response", warning);
}

void WebServer::end_connection(Server &server, int fd) const {
  if (epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, NULL) == -1)
    log("Failed to remove file descriptor from epoll", error);
  close(fd);
  std::vector<int>::iterator it = server.get_connected_clients().begin();
  for (; it != server.get_connected_clients().end(); ++it)
    if (*it == fd) {
      server.get_connected_clients().erase(it);
      break;
    }
}

void WebServer::server_routine() {
  setup_server_sockets();
  while (is_running) {
    try {
      const int num_events = epoll_wait(epoll_fd_, events_, MAX_EVENTS, 200);
      if (num_events == -1)
        log("epoll failed", warning);
      for (int i = 0; i < num_events; i++) {
        std::cout << events_[i].data.fd << std::endl;
        if (events_[i].events & EPOLLIN) {
          const int fd = events_[i].data.fd;
          std::vector<Server>::iterator server_it = find_server(fd);
          if (server_it == servers_.end()) {
            log("Failed to find server", error);
            continue;
          }
          is_server(fd) ? accept_connection(*server_it, fd)
                        : handle_connection(*server_it, fd);
        }
        if (events_[i].events & EPOLLOUT)
          log("EPOLLOUT", info);
        if (events_[i].events & EPOLLERR)
          log("EPOLLERR", warning);
        if (events_[i].events & EPOLLHUP)
          log("EPOLLHUP", warning);
      }
    } catch (...) {
      exit(1);
    }
  }
}

#pragma endregion