#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "server.hpp"
#include "user.hpp"

class User;

class Channel {
	public:
		Channel(std::string channel_name, User &user);

		std::string &getTopic();
		void setTopic(std::string &topic);
		std::vector<User> &getUsers();
		std::string &getChannelName();
		void setChannelName(std::string &channel_name);

	private:
		std::string m_channel_name;
		std::string m_password;
		std::string m_topic;
		std::vector<User> m_users;
		std::vector<std::string> m_operators;

};