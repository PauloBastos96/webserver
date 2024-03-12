#include <sys/socket.h>
#include <netinet/in.h>
#include "webserv.hpp"

int main(const int ac, const char **av) {
    (void) av;
    try {
        logger logger;
        if (ac > 2)
            logger::log("webserv: too many arguments", error);
        // Parse the configuration file
        //Config config(ac == 1 ? "configs/default.conf" : av[1]);
        // Setup the server
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

		vector<Server> *servers;
		if (ac == 2)
		{
			servers = AConfigParser::parseConfigFile(av[1]);
		}
		else
		{
			servers = AConfigParser::parseConfigFile("configs/default.conf");
		}
		AConfigParser::debugDisplayServerConfigs(*servers);
		delete servers;
    } 
    catch (std::exception &e) {
		logger::log(e.what(), error);
        return 1;
    }
}