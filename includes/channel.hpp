#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "server.hpp"
#include "user.hpp"

class Server;
class User;

class Channel {
	public:
		Channel();
		Channel(std::string channel_name, User &user);

		// GETTERS & SETTERS
		// m_topic
		std::string &getTopic();
		void setTopic(std::string &topic);
		// m_users
		std::vector<User> &getUsers();
		// m_operators
		std::vector<User> &getOperators();
		// m_channel_name
		std::string &getChannelName();
		void setChannelName(std::string &channel_name);
		// m_pasword
		std::string &getPassword();
		void setPassword(std::string &password);


		void addNewUser(User &user);
		void broadcastMsg(Server *server, User &user_to_ignore, std::string msg);

	private:
		std::string m_channel_name;
		std::string m_password;
		std::string m_topic;
		std::vector<User> m_users;
		std::vector<User> m_operators;

};