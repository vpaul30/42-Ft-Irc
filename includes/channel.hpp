#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include "server.hpp"
#include "user.hpp"

class Server;
class User;

class Channel {
	public:
		Channel();
		Channel(std::string &channel_name, std::string &nickname);

		// GETTERS & SETTERS
		// m_topic
		std::string &getTopic();
		void setTopic(std::string &topic, const std::string& nickname);
		const std::string &getTopicSetter();
		std::time_t getTimeOfTopic();
		// m_users
		std::vector<std::string> &getUsers();
		// m_operators
		std::vector<std::string> &getOperators();
		// m_invited_users
		std::vector<std::string> &getInvitedUsers();
		// m_channel_name
		std::string &getChannelName();
		void setChannelName(std::string &channel_name);
		// m_pasword
		std::string &getPassword();
		void setPassword(std::string &password);
		// m_invite_only
		bool getInviteOnly();
		void setInviteOnly(bool value);
		// m_users_limit
		int getUsersLimit();
		void setUsersLimit(int limit);


		void addNewUser(std::string &nickname);
		void addNewOperator(std::string &nickname);
		void addInvitedUser(std::string &nickname);
		void broadcastMsg(Server *server, std::string &nick_to_ignore, std::string &msg);

	private:
		std::string m_channel_name;
		std::string m_password;
		std::string m_topic;
		std::string m_topicSetter;
		std::time_t m_timeOfTopic;
		// std::vector<User> m_users;
		// std::vector<User> m_operators;
		std::vector<std::string> m_users;
		std::vector<std::string> m_operators;
		std::vector<std::string> m_invited_users;
		bool m_invite_only;
		int m_users_limit;
};