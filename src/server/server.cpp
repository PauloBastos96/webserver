#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <fcntl.h>
#include "server.hpp"
#include "location.hpp"
#include "logger.hpp"
#include "webserver.hpp"
#include "http_handler.hpp"

Server::Server() {
	port_ = 80;
	host_ = "127.0.0.1";
	config_.set_root("/");
	config_.set_max_client_body_size("1m");
	config_.set_index("index.html");
	config_.set_auto_index(false);
	socket_fd_ = -1;
}

Server::~Server() {
	if (socket_fd_ != -1)
		close(socket_fd_);
};

void Server::set_host(const std::string &host) {
	host_ = host;
}

void Server::set_port(const int &port) {
	port_ = port;
}

void Server::set_server_name(const std::string &server_name) {
	server_name_.push_back(server_name);
}

void config::set_root(const std::string &root) {
	root_ = root;
}

const int &Server::get_port() const {
	return (port_);
}

const std::string &Server::get_host() {
	return (host_);
}

const std::vector<std::string> &Server::get_server_name() {
	return (server_name_);
}

config &Server::get_config() {
	return (config_);
}

std::vector<Location> &Server::get_locations() {
	return (locations_);
}

/// @brief Setup address info struct for the server
/// @return A pointer to the address info struct
addrinfo *Server::get_addressinfo() const {
	struct addrinfo hints;
	struct addrinfo *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	std::stringstream ss;
	ss << port_;
	int s = getaddrinfo(host_.c_str(), ss.str().c_str(), &hints, &result);
	if (s != 0)
		WebServer::log("getaddrinfo failed", error);
	return (result);
}

/// @brief Setup the listening socket for the server
/// @param addrinfo The address info struct
void Server::setup_socket(struct addrinfo *addrinfo)
{
	std::stringstream ss;
	
	ss << port_;
	socket_fd_ = socket(addrinfo->ai_family, addrinfo->ai_socktype, addrinfo->ai_protocol);
	if (socket_fd_ == -1)
		WebServer::log(std::string(ERR_SOCKET) + host_ + ":" + ss.str() , error);
	int opt = 1;
	setsockopt(socket_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));
	int flags = fcntl(socket_fd_, F_GETFL, 0);
	fcntl(socket_fd_, F_SETFL, flags | O_NONBLOCK);
	if (bind(socket_fd_, addrinfo->ai_addr, addrinfo->ai_addrlen) == -1)
	{
		close(socket_fd_);
		WebServer::log(std::string(ERR_BIND) + host_ + ":" + ss.str(), error);
	}
	if (listen(socket_fd_, 10) == -1)
	{
		close(socket_fd_);
		WebServer::log(std::string(ERR_LISTEN) + host_ + ":" + ss.str(), error);
	}
	freeaddrinfo(addrinfo);
}

/// @brief Stop server
/// @param withError If true, the server stopped with an error
void Server::stop(bool withError)
{
	if (socket_fd_ != -1)
		close(socket_fd_);
	if (withError)
		WebServer::log("Server stopped with error", error);
	else
		WebServer::log("Server stopped", info);
}

void Server::run(pollfd *fds)
{
	char 				buffer[1024];
	struct sockaddr_in	client;
	socklen_t			client_size = sizeof(client);
	int bytes = 0;
	(void)fds;

	while (true)
	{
		int client_fd = accept(socket_fd_, (struct sockaddr *)&client, &client_size);
		if (client_fd == -1)
		{
			if (errno == EWOULDBLOCK || errno == EAGAIN)
				continue;
			WebServer::log("accept failed", error);
		}
		bytes = read(client_fd, buffer, 1024);
		if (bytes == -1)
			WebServer::log("read failed", error);
		if (bytes == 0)
			continue;
		buffer[bytes] = 0;
		WebServer::log(buffer, debug);
		
		HttpHandler handler(buffer, client_fd);
		handler.processRequest();
		// std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
		// write(client_fd, response.c_str(), response.length());
		// std::fstream file;
		// file.open("test_pages/index.html", std::ios::in);
		// if (file.is_open())
		// {
		// 	while (file.getline(buffer, 1024))
		// 	{
		// 		write(client_fd, buffer, strlen(buffer));
		// 	}
		// 	file.close();
		// }
		close(client_fd);
	}
}

void Server::start(void){
	struct addrinfo		*result;
	pollfd				fds[1];

	result = get_addressinfo();
	setup_socket(result);
	WebServer::log(INFO_SVR_START, info);
	run(fds);
	close(socket_fd_);
}