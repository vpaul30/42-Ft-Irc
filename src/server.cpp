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

	logMsg("Listening...");
	while (true) {
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
					User &user = m_users[m_fds[i].fd];
					if (!readMsg(user.getFd())) {
						continue;
					}
					// parse and manage message
					std::string log_msg = user.getHostname() + ":" + 
										std::to_string(user.getPort()) + " ";
					logMsg(log_msg + user.getMsgBuffer()); // temp
					std::string response = "Me: " + user.getMsgBuffer();
					send(user.getFd(), response.c_str(), response.length(), 0);
					
					user.resetMsgBuffer();
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

// returns 0 when '\n' is found
// returns 1 when '\n' is not found
// TODO: decide what to do on recv error (disconnect user or simply continue)
int Server::readMsg(int fd) {
	char recv_buffer[RECV_BUFFER_SIZE];
	int bytesRead;

	std::memset(recv_buffer, 0, RECV_BUFFER_SIZE);
	bytesRead = recv(fd, recv_buffer, RECV_BUFFER_SIZE, 0);
	if (bytesRead < 0) { // EWOULDBLOCK???
		logMsg("Recv error.");
		return 0;
	} else if (bytesRead == 0) {
		disconnectUser(fd);
		return 0;
	}
	std::string msg(recv_buffer);
	m_users[fd].appendMsgBuffer(msg);
	logMsg(msg);
	if (msg.find('\n') != std::string::npos) {
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

    strftime(buffer, sizeof(buffer), "%d-%m-%Y %H:%M:%S", timeinfo);
    std::string str(buffer);

	if (msg[msg.size() - 1] == '\n') { // temp?
		msg.pop_back();
	}

    std::cout << "\033[0;34m[" << str << "]\033[0m ";
    std::cout << msg << std::endl;
}