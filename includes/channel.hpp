#pragma once

#include <iostream>
#include <string>
#include <vector>

class Channel {
	public:


	private:
		std::string channelName;
		bool hasPassword;
		std::string password;
		std::vector<std::string> users;
		std::vector<std::string> operators;

};