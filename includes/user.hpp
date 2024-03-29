#pragma once

#include <iostream>
#include <string>
#include <netinet/in.h>

class User {
	public:
		User();
		User(int fd, std::string hostname, int port);

		// GETTERS & SETTERS
		// m_fd
		int getFd();
		void setFd(int);
		// m_addr
		// sockaddr_in *getAddr();
		// void setAddr(sockaddr_in &addr);
		// m_addr_size;
		// socklen_t *getAddrSize();
		// void setAddrSize(socklen_t size);
		// m_msg_buffer
		std::string &getMsgBuffer();
		void setMsgBuffer(std::string &msg_buffer);
		// m_rpl_buffer
		std::string &getRplBuffer();
		void setRplBuffer(std::string &rpl_buffer);
		// m_hostname
		std::string &getHostname();
		void setHostname(std::string &hostname);
		// m_port
		int getPort();
		void setPort(int port);
		// m_is_authorised
		bool getIsAuthorised();
		void setIsAuthorised(bool value);
		// m_nickname
		std::string &getNickname();
		void setNickname(std::string &nickname);
		// m_username
		std::string &getUsername();
		void setUsername(std::string &username);
		// m_realname
		std::string &getRealname();
		void setRealname(std::string &realname);
		// m_is_valid_pass
		bool getIsPassValid();
		void setIsPassValid(bool value);
		// m_must_disconnect
		bool getMustDisconnect();
		void setMustDisconnect(bool value);
		// m_is_operator
		bool getIsOperator();
		void setIsOperator(bool value);
		// m_operator_id
		int getOperatorId();
		void setOperatorId(int value);

		// FUNCTIONS
		void appendMsgBuffer(std::string &str);
		void appendRplBuffer(std::string &str);

	private:
		int m_fd;
		// sockaddr_in m_addr;
		// socklen_t m_addr_size;
		std::string m_hostname;
		int m_port;

		bool m_must_disconnect;
		bool m_is_authorised;
		bool m_is_pass_valid;
		bool m_is_operator;
		int m_operator_id;
		std::string m_nickname;
		std::string m_username;
		std::string m_realname;
		std::string m_active_channel;
		std::string m_msg_buffer;
		std::string m_rpl_buffer;

};