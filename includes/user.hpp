#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>

class User {
	public:
		User();
		User(int fd, std::string hostname, int port);

		int getFd();
		void setFd(int);
		// sockaddr_in *getAddr();
		// void setAddr(sockaddr_in &addr);
		// socklen_t *getAddrSize();
		// void setAddrSize(socklen_t size);
		std::string getMsgBuffer();
		void setMsgBuffer(std::string &msg_buffer);
		std::string getHostname();
		void setHostname(std::string &hostname);
		int getPort();
		void setPort(int port);

		void reply(std::string &msg);
		void appendMsgBuffer(std::string &str);
		void resetMsgBuffer();

	private:
		int m_fd;
		// sockaddr_in m_addr;
		// socklen_t m_addr_size;
		std::string m_hostname;
		int m_port;

		bool m_is_authorised;
		std::string m_username;
		std::string m_active_channel;
		std::string m_msg_buffer;

};