#include "../includes/server.hpp"

extern bool server_loop;

Server::Server(int port, std::string password) : m_port(port), m_password(password) {
	time_t rawtime;
    tm *timeinfo;
    char buffer[80];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeinfo);
	m_time_of_start = buffer;

	m_listening_socket = 0;

}

// Creates a listening socket on localhost:<port>	
int Server::setup() {
	logMsg("Starting server...", SERVER);

	// read from oper.conf
	std::ifstream oper_file;
	oper_file.open("oper.conf");
	if (oper_file.good() == false) {
		errorMsg("Could't open oper.conf file.");
	} else {
		std::string line;
		int id_counter = 1;
		size_t pos;
		OperInfo oper_info;
		while (std::getline(oper_file, line)) {
			oper_info.id = id_counter;
			if ((pos = line.find(' ')) == std::string::npos) {
				std::string str = "Wrong data for operator " + intToString(id_counter);
				errorMsg(str);
				continue;
			}
			oper_info.name = line.substr(0, pos);
			line.erase(0, pos + 1);
			if ((pos = line.find(' ')) == std::string::npos) {
				std::string str = "Wrong data for operator " + intToString(id_counter);
				errorMsg(str);
				continue;
			}
			oper_info.password = line.substr(0, pos);
			line.erase(0, pos + 1);
			if (line.empty() == true) {
				std::string str = "Wrong data for operator " + intToString(id_counter);
				errorMsg(str);
				continue;
			}
			oper_info.host = line;
			m_oper_info.push_back(oper_info);			
			// std::cout << "name: " << oper_info.name << ", pass: " << oper_info.password << ", host: " << oper_info.host << std::endl;
			id_counter++;
		}
	}

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

// Main loop of the server.
// With poll() waiting for events on fds (listening_socket and users)
// On returned POLLIN event either accepts a new connection or reads a message.
// On returned POLLOUT event sends reply to user and sometimes disconnect that user (e.g. in case of a QUIT command)
int Server::loop() {
	int poll_res;
	pollfd new_fd;

	new_fd.fd = m_listening_socket;
	new_fd.events = POLLIN;
	new_fd.revents = 0;
	m_fds.push_back(new_fd);

	logMsg("Listening...", SERVER);
	while (server_loop) {
		poll_res = poll((pollfd *)&m_fds[0], m_fds.size(), -1);
		if (poll_res < 0 && errno != EINTR) {
			errorMsg("poll error.");
			break;
		}
		for (size_t i = 0; i < m_fds.size(); i++) {
			if (m_fds[i].revents & POLLIN) {
				// listening socket
				if (m_fds[i].fd == m_listening_socket) {
					// create client and accept
					if (acceptUser() == -1) {
						errorMsg("Error accepting user.");
					}
				} else {
					User &user = m_users[m_fds[i].fd];
					if (!readMsg(user.getFd())) { //
						break;
					}
					std::string log_msg = user.getHostname() + ":" + intToString(user.getPort()) + " ";
					logMsg(log_msg + user.getMsgBuffer(), SERVER);
					processUserMsg(user); // parse and manage messag
				}
				break;
			} else if (m_fds[i].revents & POLLOUT) {
				User &user = m_users[m_fds[i].fd];
			 	// std::cout << "rpl buf(pollout): " << user.getRplBuffer() << " user: " << user.getNickname() << std::endl;
				if (!user.getRplBuffer().empty()) {
					std::string &rpl_buffer = user.getRplBuffer();
					size_t bytes_sent = send(user.getFd(), rpl_buffer.c_str(), rpl_buffer.size(), 0);
					// std::cout << "rpl buf size = " << rpl_buffer.size() << " bytes_sent = " << bytes_sent << std::endl;
					logMsg(rpl_buffer.substr(0, bytes_sent), CLIENT);
					rpl_buffer.erase(0, bytes_sent);
					if (user.getMustDisconnect() == true) // after sending the reply check if user must be disconnected
						disconnectUser(user.getFd());
					if (rpl_buffer.empty())
						m_fds[i].events = POLLIN;
				}
				else {
					// std::cout << "EMPTY POLLOUT!!!\n";
					User &user = m_users[m_fds[i].fd];
					// std::cout << "USER: " << user.getNickname() << std::endl;
					if (user.getRplBuffer().empty())
						m_fds[i].events = POLLIN;
				}
				break;
			}
		}
	}
	cleanup();
	return 0;
}

int Server::processUserMsg(User &user) {
	std::string &user_msg = user.getMsgBuffer();
	std::vector<std::string> messages;
	splitMessages(messages, user_msg); // split message into separate commands (in case there is more than 1)

	for (size_t i = 0; i < messages.size(); i++) { // parse and execute each command
		MsgInfo msg_info;
		parseMsg(messages[i], msg_info); // parse command into MsgInfo struct

		// if user is not registered server executes only PASS, NICK, USER (mb change later)
		if (user.getIsAuthorised() == false) {
			std::string cmd = msg_info.cmd;
			if (cmd != "PASS" && cmd != "NICK" && cmd != "USER") {
				continue;
			}
			executeCommand(user, msg_info);
			// once server has nickname and username it tries to register user
			if (!user.getNickname().empty() && !user.getUsername().empty()) {
				if (user.getIsPassValid() && validateUsername(user.getUsername())) { // valid registration
					user.setIsAuthorised(true);
					std::string reply = registrationMessage(*this, user);
					addRplAndPollout(user, reply);
				} else { // send error message and disconnect user
					user.setMustDisconnect(true);
					std::string reply = ERR_REGISTRATION;
					addRplAndPollout(user, reply);
				}
			}
		} else { // if user is registered simply executes any command
			executeCommand(user, msg_info);
		}
	}

	return 0;
}

int Server::executeCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.cmd == "PASS") {
		passCommand(user, msg_info);
	} else if (msg_info.cmd == "NICK") {
		nickCommand(user, msg_info);
	} else if (msg_info.cmd == "USER") {
		userCommand(user, msg_info);
	} else if (msg_info.cmd == "PRIVMSG") {
		privmsgCommand(user, msg_info);
	} else if (msg_info.cmd == "JOIN") {
		joinCommand(user, msg_info);
	} else if (msg_info.cmd == "TOPIC") {
		topicCommand(user, msg_info);
	} else if (msg_info.cmd == "PART") {
		partCommand(user, msg_info);
	} else if (msg_info.cmd == "NOTICE") {
		noticeCommand(user, msg_info);
	} else if (msg_info.cmd == "OPER") {
		operCommand(user, msg_info);
	} else if (msg_info.cmd == "INVITE") {
		inviteCommand(user, msg_info);
	}
	else {
		// unknown command: ...
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
	logMsg(msg, SERVER);

	m_users.insert(std::pair<int, User>(user_fd, new_user));
	pollfd poll_fd;
	poll_fd.fd = user_fd;
	poll_fd.events = POLLIN;
	poll_fd.revents = 0;
	m_fds.push_back(poll_fd);

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
	std::string reply = prefix(m_users[fd].getNickname(), m_users[fd].getUsername(), m_users[fd].getHostname());
	reply += QUIT(m_users[fd].getNickname());

	removeUserFromChannels(m_users[fd].getNickname(), reply);
	removeEmptyChannels();
	m_users.erase(fd);
	close(fd);
	logMsg(msg, SERVER);
}

void Server::removeUserFromChannels(std::string &nickname, std::string &reply) {
	std::map<std::string, Channel>::iterator channels_it = m_channels.begin();
	std::vector<std::string>::iterator nicknames_it;
	for (; channels_it != m_channels.end(); channels_it++) {
		nicknames_it = channels_it->second.getUsers().begin();
		for (; nicknames_it != channels_it->second.getUsers().end(); nicknames_it++) {
			if (*nicknames_it == nickname) {
				channels_it->second.broadcastMsg(this, nickname, reply);
				channels_it->second.getUsers().erase(nicknames_it);
				break;
			}
		}
		nicknames_it = channels_it->second.getOperators().begin();
		for (; nicknames_it != channels_it->second.getOperators().end(); nicknames_it++) {
			if (*nicknames_it == nickname) {
				channels_it->second.getOperators().erase(nicknames_it);
				break;
			}
		}
		nicknames_it = channels_it->second.getInvitedUsers().begin();
		for (; nicknames_it != channels_it->second.getInvitedUsers().end(); nicknames_it++) {
			if (*nicknames_it == nickname) {
				channels_it->second.getInvitedUsers().erase(nicknames_it);
				break;
			}
		}
	}
}

void Server::removeEmptyChannels() {
	std::map<std::string, Channel>::iterator channels_it = m_channels.begin();
	while (channels_it != m_channels.end()) {
		if (channels_it->second.getUsers().empty() && channels_it->second.getOperators().empty()) {
			std::cout << channels_it->first << " is empty.\n";
			m_channels.erase(channels_it);
			channels_it = m_channels.begin();
			continue;
		}
		channels_it++;
	}
}

// returns 0 when '\r\n' is not found
// returns 1 when '\r\n' is found
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
	// logMsg(msg, SERVER);
	if (msg.find("\r\n") != std::string::npos) {
		return 1;
	}
	return 0;
}

void Server::logMsg(std::string msg, int mode) {
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
	if (mode == SERVER) {
    	std::cout << "\033[0;34m[" << str << "]\033[0m ";
	} else {
    	std::cout << "\033[0;32m[" << str << "]\033[0m ";
	}
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

std::map<int, User> &Server::getUsers() { return m_users; }

std::string &Server::getTimeOfStart() { return m_time_of_start; }

void Server::addPolloutToPollfd(int user_fd) {
	std::vector<pollfd>::iterator it = m_fds.begin();
	for (; it != m_fds.end(); it++) {
		if (it->fd == user_fd)
			it->events = POLLIN | POLLOUT;
	}
}

void Server::addRplAndPollout(User &user, std::string &reply) {
	user.appendRplBuffer(reply);
	addPolloutToPollfd(user.getFd());
}

User *Server::getUserByNickname(std::string nickname) {
	std::map<int, User>::iterator it = m_users.begin();
	for (; it != m_users.end(); it++) {
		if (it->second.getNickname() == nickname) {
			return &(it->second);
		}
	}
	return NULL;
}

std::map<std::string, Channel> &Server::getChannels() { return m_channels; }

// Channel &Server::getChannelByName(std::string &channel_name) {
// 	Channel channel = m_channels.find(channel_name);
// 	return channel;
// }