// #include <iostream>
// #include <string>
#include "../includes/server.hpp"

bool server_loop = true;

static void signalHandler(int signal) {
	(void)signal;
	server_loop = false;
	std::cout << std::endl;
}

int main(int argc, char **argv) {

	// check arguments (port, password)
	if (argc != 3) {
		Server::errorMsg("You need to pass 2 arguments (port and password).");
		return 1;
	}
	int port = atoi(argv[1]);
	if (port <= 0 || port > 65535) {
		Server::errorMsg("Port should be in range 1-65535.");
		return 1;
	}
	if (argv[2][0] == '\0') {
		Server::errorMsg("Password shouldn't be empty.");
		return 1;
	}
	// start server
	Server server(port, argv[2]);
	if (server.setup() == -1) {
		Server::errorMsg("Server setup error.");
		return 0;
	}

	signal(SIGINT, signalHandler);
	server.loop();
	
	Server::logMsg("Server shutting down...");

	return 0;
}