#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <location.hpp>
#include <server.hpp>
#include <config.hpp>
#include <webserver.hpp>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>
#include <poll.h>

//TESTING
void	setupServer()
{
	struct addrinfo *result;
	struct addrinfo hints;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = 0;
	int s = getaddrinfo("127.0.0.1", "8080", &hints, &result);
	if (s != 0)
		WebServer::log("webserv: getaddrinfo failed", error);
	int	socket_fd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	if (socket_fd == -1)
		WebServer::log("webserv: socket failed", error);
	
	if (bind(socket_fd, result->ai_addr, result->ai_addrlen) == -1)
		WebServer::log("webserv: bind failed", error);
	listen(socket_fd, 10);
	struct sockaddr_in client;
	socklen_t client_size = sizeof(client);
	int client_fd = accept(socket_fd, (struct sockaddr *)&client, &client_size);
	if (client_fd == -1)
		WebServer::log("webserv: accept failed", error);
	char buffer[1024];
	pollfd fds[1];
	fds[0].fd = client_fd;
	fds[0].events = POLLIN;
	while (true)
	{
		int ret = poll(fds, 1, 1000);
		if (ret == -1)
			WebServer::log("webserv: poll failed", error);
		if (fds[0].revents & POLLIN)
		{
			int bytes = read(client_fd, buffer, 1024);
			if (bytes == -1)
				WebServer::log("webserv: recv failed", error);
			if (bytes == 0)
				break;
			buffer[bytes] = 0;
			std::cout << buffer << std::endl;
			break;
		}
	}
	close(client_fd);
	close(socket_fd);
	freeaddrinfo(result);
}

int main(const int ac, const char **av) {
	try {
		WebServer webserver;
		if (ac > 2)
			WebServer::log("webserv: too many arguments", error);
		// Parse the configuration file
		webserver.config_servers(ac == 1 ? "configs/default.conf" : av[1]);
		//config::display_configs(webserver.get_servers());
		// Setup the servers

		setupServer();
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
	} catch (const std::runtime_error &e) {
		return 1;
	} catch (const std::exception &e) {
		std::cerr << RED << "[FATAL]:	" << e.what() << RESET << std::endl;
		return 1;
	}
}