#include <vector>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include "server.hpp"
#include "location.hpp"
#include "logger.hpp"
#include "webserver.hpp"
#include <fcntl.h>

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
	fcntl(socket_fd_, fcntl(socket_fd_, F_GETFL, 0) | O_NONBLOCK);
	if (bind(socket_fd_, addrinfo->ai_addr, addrinfo->ai_addrlen) == -1)
	{
		close(socket_fd_);
		WebServer::log(std::string(ERR_BIND) + host_ + ":" + ss.str(), error);
	}
	listen(socket_fd_, 20);
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

	while (true)
	{
		fds[0].fd = socket_fd_;
		fds[0].events = POLLIN;
		int ret = poll(fds, 1, 1000);
		if (ret == -1)
			stop(true);
		if (fds[0].revents & POLLIN)
		{
			int client_fd = accept(socket_fd_, (struct sockaddr *)&client, &client_size);
			if (client_fd == -1)
				stop(true);
			fds[0].fd = client_fd;
			fds[0].events = POLLIN | POLLOUT;
			ret = poll(fds, 1, 1000);
			if (ret == -1)
			{
				close(client_fd);
				stop(true);
			}
			if (fds[0].revents & POLLIN)
			{
				bytes = read(client_fd, buffer, 1024);
				if (bytes == -1)
					WebServer::log("read failed", error);
				if (bytes == 0)
					continue;
				buffer[bytes] = 0;
				WebServer::log(buffer, debug);
				if (fds[0].revents & POLLOUT)
				{
					std::string response = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";
					write(client_fd, response.c_str(), response.length());
					std::fstream file;
					file.open("test_pages/index.html", std::ios::in);
					if (file.is_open())
					{
						while (file.getline(buffer, 1024))
						{
							write(client_fd, buffer, strlen(buffer));
						}
						file.close();
					}
					close(client_fd);
					//close(socket_fd_);
					//socket_fd_ = -1;
					//break;
				}
			}
		}
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