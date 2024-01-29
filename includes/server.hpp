#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <poll.h>
#include <vector>
#include <map>
#include <time.h>
#include <csignal>
#include <cerrno>
#include "channel.hpp"
#include "user.hpp"
#include "utils.hpp"

#define RECV_BUFFER_SIZE 1024

class Server {
	public:
		Server(int port, std::string password);
		
		static void logMsg(std::string msg);
		static void errorMsg(std::string msg);
		
		int setup();
		int loop();
		void cleanup();

		int acceptUser();
		void disconnectUser(int fd);
		int readMsg(int fd);
	private:
		// arguments
		int m_port;
		std::string m_password;

		// server info
		int m_listening_socket;
		// sockaddr_in m_myaddr;

		// data
		std::vector<pollfd> m_fds;
		std::map<int, User> m_users;
		std::vector<Channel> m_channels;

		// limits
		// const int max_connections;
};