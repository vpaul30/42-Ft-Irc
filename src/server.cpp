#include "../includes/server.hpp"

extern bool server_loop;

Server::Server(int port, std::string password) : m_port(port), m_password(password) {
	m_listening_socket = 0;
}

// creates a listening socket (socket() and bind())
int Server::setup() {
	logMsg("Starting server...");
	m_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listening_socket == -1) {
		errorMsg("Cannot create socket.");
		return -1;
	}

	int optval = 1;
	if (setsockopt(m_listening_socket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval))) {
		errorMsg("Cannot set socket options.");
		return -1;
	}

	sockaddr_in my_addr;
	const uint16_t port = m_port;
	memset(&my_addr, 0, sizeof(my_addr));
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &my_addr.sin_addr);

	if (bind(m_listening_socket, (sockaddr *)&my_addr, sizeof(my_addr)) == -1) {
		errorMsg("Cannot bind to ip address.");
		return -1;
	}

	if (listen(m_listening_socket, 1) == -1) {
		errorMsg("Cannot listen.");
		return -1;
	}

	return 0;
}

// main server loop, waits for activity on one of fds to do one of the following:
//	1. accept a new connection
//	2. communicate with user (authorise on server, create new channel, connect to channel etc.)
//	3. broadcast user's message to other users of current channel
int Server::loop() {
	int poll_res;
	pollfd new_fd;

	new_fd.fd = m_listening_socket;
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	m_fds.push_back(new_fd);

	logMsg("Listening...");
	while (server_loop) {
		poll_res = poll((pollfd *)&m_fds[0], m_fds.size(), -1);
		if (poll_res < 0 && errno != EINTR) {
			errorMsg("poll error.");
		}
		for (size_t i = 0; i < m_fds.size(); i++) {
			if (m_fds[i].revents & POLLIN) {
				// listening socket
				if (m_fds[i].fd == m_listening_socket) {
					// create client and accept
					if (acceptUser() == -1) {
						errorMsg("Error accepting user.");
						continue;
					}
				} else {
					User &user = m_users[m_fds[i].fd];
					if (!readMsg(user.getFd())) {
						continue;
					}
					std::string log_msg = user.getHostname() + ":" + 
										intToString(user.getPort()) + " ";
					logMsg(log_msg + user.getMsgBuffer()); // temp
					// parse and manage message
					processUserMsg(user);

					std::string response = "Me: " + user.getMsgBuffer();
					// user.resetMsgBuffer();
				}
			}
		}
	}
	cleanup();
	return 0;
}

int Server::processUserMsg(User &user) {
	// split into commands first (as some clients might send more than one command in one message)
	// execute all cmds
	
	std::string &user_msg = user.getMsgBuffer();
	std::vector<std::string> messages;
	splitMessages(messages, user_msg);

	for (size_t i = 0; i < messages.size(); i++) {
		// parse each command and execute
		// parse message into MsgInfo struct
		MsgInfo msg_info;
		parseMsg(messages[i], msg_info);
		
	}

	// check if user is authorised
	// MsgInfo msg_info;
	// if (parseMsg(msg, msg_info) == 1) {
	// 	errorMsg("Parse message error.");
	// }
	
	if (user.getIsAuthorised() == false) {
		;
	}
	return 0;
}

int Server::splitMessages(std::vector<std::string> &messages, std::string &user_msg) {
	size_t pos = 0;
	std::string delim = "\r\n";
	while ((pos = user_msg.find(delim)) != std::string::npos) {
		messages.push_back(user_msg.substr(0, pos));
		user_msg.erase(0, pos + delim.size());
	}
	return 0;
}

// prefix -> starts with ':' and ends with first ' ' (space)
// command -> one word after prefix, can have any amount of spaces before and after
// parameters -> any amount of words after command divided by any amount of spaces???
int Server::parseMsg(std::string &msg, MsgInfo &msg_info) {
	size_t pos;
	size_t pos2;
	char delim = ' ';
	if (msg[0] == ':') { // prefix
		if ((pos = msg.find(delim)) != std::string::npos) {
			msg_info.prefix = msg.substr(0, pos);
			msg.erase(0, pos + 1);
		}
	}
	if ((pos = msg.find_first_not_of(' ')) != std::string::npos) {
		pos2 = msg.find_first_of(' ', pos);
		msg_info.cmd = msg.substr(pos, pos2 - pos);
		msg.erase(0, pos2);
	}
	if ((pos = msg.find_first_not_of(' ')) != std::string::npos) {
		pos2 = msg.find_last_not_of(' ');
		msg_info.params = msg.substr(pos, pos2 - pos + 1);
		msg.erase(0, pos2);
	}

	// std::cout << "prefix = " << msg_info.prefix << ".\n";
	// std::cout << "cmd = " << msg_info.cmd << ".\n";
	// std::cout << "params = " << msg_info.params << ".\n";
	return 0;
}

int Server::acceptUser() {
	sockaddr_in addr = {};
	socklen_t size = sizeof(addr);

	int user_fd = accept(m_listening_socket, (sockaddr *) &addr, &size);
	if (!user_fd) {
		errorMsg("Couldn't accept new user.");
		return -1;
	}

	if (fcntl(user_fd, F_SETFL, O_NONBLOCK) == -1) {
		errorMsg("fcntl() error.");
		return -1;
	}

	char hostname_buff[NI_MAXHOST];
	if (getnameinfo((sockaddr *) &addr, size, hostname_buff, NI_MAXHOST, NULL, 0, NI_NAMEREQD) != 0) {
		errorMsg("Couldn't get new user's hostname.");
		return -1;
	}

	std::string hostname(hostname_buff);
	User new_user(user_fd, hostname, ntohs(addr.sin_port));

	std::string msg = new_user.getHostname() + ":" + intToString(new_user.getPort()) + " connected.";
	logMsg(msg);

	m_users.insert(std::pair<int, User>(user_fd, new_user));
	pollfd poll_fd;
	poll_fd.fd = user_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
	m_fds.push_back(poll_fd);

	std::string welcome_msg = "Welcome!\n";
	send(user_fd, welcome_msg.c_str(), welcome_msg.size(), 0);

	return 0;
}

void Server::disconnectUser(int fd) {
	std::vector<pollfd>::iterator it = m_fds.begin();
	while (it != m_fds.end()) {
		if (it->fd == fd) {
			m_fds.erase(it);
			break;
		}
		it++;
	}
	std::string msg = m_users[fd].getHostname() + ":" + intToString(m_users[fd].getPort()) + " disconnected.";
	m_users.erase(fd);
	close(fd);
	logMsg(msg);
}

// returns 0 when '\n' is found
// returns 1 when '\n' is not found
// TODO: decide what to do on recv error (disconnect user or simply continue)
int Server::readMsg(int fd) {
	char recv_buffer[RECV_BUFFER_SIZE];
	int bytesRead;

	std::memset(recv_buffer, 0, RECV_BUFFER_SIZE);
	bytesRead = recv(fd, recv_buffer, RECV_BUFFER_SIZE, 0);
	if (bytesRead < 0 && errno == EWOULDBLOCK) {
		// std::cout << "EWOULDBLOCK\n";
		return 0;
	}
	if (bytesRead < 0) {
		errorMsg("Recv error.");
		return 0;
	}
	if (bytesRead == 0) {
		disconnectUser(fd);
		return 0;
	}
	std::string msg(recv_buffer);
	m_users[fd].appendMsgBuffer(msg);
	// logMsg(msg);
	if (msg.find("\r\n") != std::string::npos) {
		return 1;
	}
	return 0;
}

void Server::logMsg(std::string msg) {
	time_t      rawtime;
    struct tm   *timeinfo;
    char        buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    std::string str(buffer);

	if (msg.find("\r\n") != std::string::npos) {
		msg.erase(msg.size() - 2, 2);
	}
    std::cout << "\033[0;34m[" << str << "]\033[0m ";
    std::cout << msg << std::endl;
}

 void Server::errorMsg(std::string msg) {
	time_t      rawtime;
    struct tm   *timeinfo;
    char        buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
    std::string str(buffer);

    std::cerr << "\033[0;31m[" << str << "] ";
    std::cerr << msg << "\033[0m";
	if (msg[msg.size() - 1] != '\n') {
		std::cout << std::endl;
	}
 }

void Server::cleanup() {
	// close all sockets
	std::vector<pollfd>::iterator it = m_fds.begin();
	for (; it != m_fds.end(); it++) {
		close(it->fd);
	}
}
