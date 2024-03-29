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
#include <fstream>
#include <cerrno>
#include "channel.hpp"
#include "user.hpp"
#include "utils.hpp"
#include "replies.hpp"


#define RECV_BUFFER_SIZE 1024
#define SERVER 10 // logMsg() to change color between server logs and client replies
#define CLIENT 11 // logMsg() to change color between server logs and client replies

struct MsgInfo {
	std::string prefix;
	std::string cmd;
	std::string params;
};

struct OperInfo {
	int id;
	std::string name;
	std::string password;
	std::string host;
};

class Channel;

class Server {
	public:
		Server(int port, std::string password);

		// mode SERVER -> blue color, mode CLIENT -> green color
		static void logMsg(std::string msg, int mode);
		static void errorMsg(std::string msg);
		
		int setup();
		int loop();
		void cleanup();

		std::map<int, User> &getUsers();

		// private???
		int acceptUser();
		void disconnectUser(int fd);
		int readMsg(int fd);
		int processUserMsg(User &user);
		int parseMsg(std::string &msg, MsgInfo &msg_info);
		int splitMessages(std::vector<std::string> &messages, std::string &user_msg);
		void addPolloutToPollfd(int user_fd);
		void addRplAndPollout(User &user, std::string &reply);
		std::string &getTimeOfStart();
		void removeUserFromChannels(std::string &nickname, std::string &reply);
		void removeEmptyChannels();
		void chatbot(std::map<int, User>& users);

		User *getUserByNickname(std::string nickname);
		std::map<std::string, Channel> &getChannels();
		// Channel &getChannelByName(std::string &channel_name);

		// === COMMANDS ===
		int executeCommand(User &user, MsgInfo &msg_info);
		int passCommand(User &user, MsgInfo &msg_info);
		int nickCommand(User &user, MsgInfo &msg_info);
		int userCommand(User &user, MsgInfo &msg_info);
		int privmsgCommand(User &user, MsgInfo &msg_info);
		int noticeCommand(User &user, MsgInfo &msg_info);
		int joinCommand(User &user, MsgInfo &msg_info);
		int partCommand(User &user, MsgInfo msg_info);
		int topicCommand(User &user, MsgInfo &msg_info);
		int operCommand(User &user, MsgInfo &msg_info);
		int inviteCommand(User &user, MsgInfo &msg_info);
		int kickCommand(User &user, MsgInfo &msg_info);
		int modeCommand(User &user, MsgInfo &msg_info);

	private:
		// arguments
		int m_port;
		std::string m_password;

		// server info
		int m_listening_socket;
		// sockaddr_in m_myaddr;

		// data
		std::string m_time_of_start;
		std::vector<pollfd> m_fds;
		std::map<int, User> m_users;
		std::map<std::string, Channel> m_channels;
		std::vector<OperInfo> m_oper_info;

		// limits
		// const int max_connections;
};
