#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>
#include <vector>
#include <location.hpp>
#include <server.hpp>
#include <config.hpp>
#include <webserver.hpp>
#include <signal.h>

//TESTING
void	runServers(WebServer &webserver)
{
	webserver.get_servers().at(0).start();
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

		runServers(webserver);

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
		if (!strstr(e.what(), "[ERROR]"))
			std::cerr << RED << "[FATAL]:	" << e.what() << RESET << std::endl;
		return 1;
	} catch (const std::exception &e) {
		std::cerr << RED << "[FATAL]:	" << e.what() << RESET << std::endl;
		return 1;
	}
}