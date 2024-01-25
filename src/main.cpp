#include <iostream>
#include <string>

#include "../includes/server.hpp"
#include "../includes/user.hpp"
#include "../includes/channel.hpp"

int main(int argc, char **argv) {

	// check arguments (port, password)
	if (argc != 3) {
		std::cerr << "You need to pass 2 arguments (port and password).\n";
		return 1;
	}
	int port = std::stoi(argv[1]);
	if (port <= 0 || port > 65535) {
		std::cerr << "Port should be in range 1-65535.\n";
		return 1;
	}
	if (argv[2][0] == '\0') {
		std::cerr << "Password shouldn't be empty.\n";
		return 1;
	}
	// start server
	Server server(port, argv[2]);
	if (server.setup() == -1) {
		std::cerr << "Server setup error.\n";
		return 0;
	}
	server.loop();
	//

	return 0;
}