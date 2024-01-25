#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <map>
#include <time.h>
#include "channel.hpp"
#include "user.hpp"

#define RECV_BUFFER_SIZE 1024

class Server {
	public:
		Server(int port, std::string password);
		
		int setup();
		int loop();

		int acceptUser();
		void disconnectUser(int fd);
		int readMsg(int fd);
		void logMsg(std::string msg);
	private:
		// arguments
		int m_port;
		std::string m_password;

		// server info
		int m_listening_socket;
		sockaddr_in m_myaddr;

		// data
		std::vector<pollfd> m_fds;
		std::map<int, User> m_users;
		std::vector<Channel> m_channels;

		// limits
		const int max_connections = 5;

};