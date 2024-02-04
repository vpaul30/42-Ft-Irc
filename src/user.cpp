#include "../includes/user.hpp"

User::User() {
	m_fd = 0;
	m_hostname = "";
	m_port = 0;
	m_is_authorised = false;
	m_is_pass_valid = false;
	m_username = "";
	m_nickname = "";
	m_realname = "";
	m_active_channel = "";
	m_msg_buffer = "";
	m_rpl_buffer = "";
}

User::User(int fd, std::string hostname, int port)
	: m_fd(fd), m_hostname(hostname), m_port(port) {
	m_is_authorised = false;
	m_is_pass_valid = false;
	m_username = "";
	m_nickname = "";
	m_realname = "";
	m_active_channel = "";
	m_msg_buffer = "";
	m_rpl_buffer = "";
}


int User::getFd() { return m_fd; }
void User::setFd(int value) { m_fd = value; }

// sockaddr_in *User::getAddr() { return &m_addr; }
// void User::setAddr(sockaddr_in &addr) { m_addr = addr; }

// socklen_t *User::getAddrSize() { return &m_addr_size; }
// void User::setAddrSize(socklen_t size) { m_addr_size = size; }

std::string &User::getHostname() { return m_hostname; }
void User::setHostname(std::string &hostname) { m_hostname = hostname; }

int User::getPort() { return m_port; }
void User::setPort(int port) { m_port = port; }

std::string &User::getMsgBuffer() { return m_msg_buffer; }
void User::setMsgBuffer(std::string &msg_buffer) { m_msg_buffer = msg_buffer; }

std::string &User::getRplBuffer() { return m_rpl_buffer; }
void User::setRplBuffer(std::string &rpl_buffer) { m_rpl_buffer = rpl_buffer; }

bool User::getIsAuthorised() { return m_is_authorised; }
void User::setIsAuthorised(bool value) { m_is_authorised = value; }

std::string &User::getNickname() { return m_nickname; }
void User::setNickname(std::string &nickname) { m_nickname = nickname; }

std::string &User::getUsername() { return m_username; }
void User::setUsername(std::string &username) { m_username = username; }

std::string &User::getRealname() { return m_realname; }
void User::setRealname(std::string &realname) { m_realname = realname; }

bool User::getIsPassValid() { return m_is_pass_valid; }
void User::setIsPassValid(bool value) { m_is_pass_valid = value; }

void User::reply(std::string &msg) {

	if (send(m_fd, (void *)&msg, msg.size(), 0) == -1) {
		// send() error
		std::cout << "Error sending a message.\n";
		return;
	}
}

void User::appendMsgBuffer(std::string &str) {
	m_msg_buffer += str;
}

void User::resetMsgBuffer() {
	m_msg_buffer = "";
}

void User::appendRplBuffer(std::string &str) {
	m_rpl_buffer += str;
}

