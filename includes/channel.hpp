#pragma once

#include <iostream>
#include <string>
#include <vector>

class Channel {
	public:


	private:
		std::string m_channel_name;
		std::string m_password;
		std::string m_topic;
		std::vector<std::string> m_users;
		std::vector<std::string> m_operators;

};