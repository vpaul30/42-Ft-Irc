#include "../includes/server.hpp"

Server::Server(int port, std::string password) : m_port(port), m_password(password) {}

// creates a listening socket (socket() and bind())
int Server::setup() {
	m_listening_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (m_listening_socket == -1) {
		std::cerr << "Cannot create socket.\n";
		return -1;
	}

	const uint16_t port = m_port;

	m_myaddr.sin_family = AF_INET;
	m_myaddr.sin_port = htons(port);
	inet_pton(AF_INET, "0.0.0.0", &m_myaddr.sin_addr);

	if (bind(m_listening_socket, (sockaddr *)&m_myaddr, sizeof(m_myaddr)) == -1) {
		std::cerr << "Cannot bind to ip address.\n";
		return -1;
	}

	if (listen(m_listening_socket, 1) == -1) {
		std::cerr << "Cannot listen.\n";
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
	m_fds.push_back(new_fd);

	while (true) {
		std::cout << "poll\n";
		poll_res = poll((pollfd *)&m_fds[0], m_fds.size(), -1);
		for (size_t i = 0; i < m_fds.size(); i++) {
			if (m_fds[i].revents == POLLIN) {
				// listening socket
				if (m_fds[i].fd == m_listening_socket) {
					// create client and accept
					if (acceptUser() == -1) {
						std::cerr << "Error accepting user.\n";
						continue;
					}
				} else {
					std::string msg = readMsg(m_fds[i].fd);
					if (msg == "") {
						continue;
					}
					// parse and manage message
					std::string log_msg = m_users[m_fds[i].fd].getHostname() + ":" + 
										std::to_string(m_users[m_fds[i].fd].getPort()) + " ";
					logMsg(log_msg + msg); // temp
					std::string response = "Me: " + msg;
					send(m_fds[i].fd, response.c_str(), response.length(), 0);
				}
				continue;
			}
		}
	}

	close(m_listening_socket);
	return 0;
}

int Server::acceptUser() {
	sockaddr_in addr = {};
	socklen_t size = sizeof(addr);

	int user_fd = accept(m_listening_socket, (sockaddr *) &addr, &size);
	if (!user_fd) {
		std::cerr << "Couldn't accept new user.\n";
		return -1;
	}

	char hostname_buff[NI_MAXHOST];
	if (getnameinfo((sockaddr *) &addr, size, hostname_buff, NI_MAXHOST, NULL, 0, NI_NAMEREQD) != 0) {
		std::cerr << "Couldn't get new user's hostname.\n";
		return -1;
	}

	std::string hostname(hostname_buff);
	User new_user(user_fd, hostname, ntohs(addr.sin_port));

	std::string msg = new_user.getHostname() + ":" + std::to_string(new_user.getPort()) + " connected.";
	logMsg(msg);

	m_users.insert({user_fd, new_user});
	m_fds.push_back({user_fd, POLLIN, 0});

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
	std::string msg = m_users[fd].getHostname() + ":" + std::to_string(m_users[fd].getPort()) + " disconnected.";
	m_users.erase(fd);
	close(fd);
	logMsg(msg);
}

std::string Server::readMsg(int fd) {
	std::string msg;
	char buffer[100];
	int bytesRead;

	while (true) {
		std::memset(buffer, 0, 100);
		bytesRead = recv(fd, buffer, sizeof(buffer), 0);
		if (bytesRead < 0) { // EWOULDBLOCK???
			logMsg("Recv error.");
			return ""; // return empty string???
		} else if (bytesRead == 0) { // user disconnected
			disconnectUser(fd);
			return "";
		}
		msg += buffer;
		if (msg.find('\n') != std::string::npos) {
			break;
		}	
	}

	return msg;
}

void Server::logMsg(std::string msg) {
	time_t      rawtime;
    struct tm   *timeinfo;
    char        buffer[80];

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str(buffer);

	if (msg[msg.size() - 1] == '\n') { // temp?
		msg.pop_back();
	}

    (void)msg;
    std::cout << "\033[0;34m[" << str << "]\033[0m ";
    std::cout << msg << std::endl;
}