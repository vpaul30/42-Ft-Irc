#include "../includes/utils.hpp"
#include "../includes/server.hpp"
#include "../includes/channel.hpp"
#include "../includes/user.hpp"

std::string intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bool validateUsername(std::string &username) {
	for (size_t i = 0; i < username.size(); i++) {
		if (!std::isalnum(username[i]))
			return false;
	}
	return true;
}

std::string registrationMessage(Server &server, User &user) {
	std::string msg;
	std::string nickname = user.getNickname();

	msg = RPL_WELCOME(nickname);
	msg += RPL_YOURHOST(nickname);
	msg += RPL_CREATED(nickname, server.getTimeOfStart());
	msg += RPL_MYINFO(nickname);
	msg += RPL_ISUPPORT(nickname);
	return msg;
}

bool checkChannelExist(Server *server, std::string &channel_name) {
	std::map<std::string, Channel> &channels = server->getChannels();
	if (channels.find(channel_name) == channels.end())
		return false;
	return true;
}

bool checkUserInChannel(Server *server, std::string &channel_name, std::string &nickname) {
	std::map<std::string, Channel> &channels = server->getChannels();
	Channel &channel = channels[channel_name];

	std::vector<std::string> &users = channel.getUsers();
	for (int i = 0; i < users.size(); i++) {
		if (users[i] == nickname)
			return true;		
	}
	std::vector<std::string> &operators = channel.getOperators();
	for (int i = 0; i < operators.size(); i++) {
		if (operators[i] == nickname)
			return true;		
	}
	return false;
}

bool checkUserChannelOperator(Server *server, std::string &channel_name, std::string &nickname) {
	std::map<std::string, Channel> &channels = server->getChannels();
	Channel &channel = channels[channel_name];
	
	std::vector<std::string> &operators = channel.getOperators();
	for (int i = 0; i < operators.size(); i++) {
		if (operators[i] == nickname)
			return true;		
	}
	return false;
}

bool checkUserExist(Server *server, std::string &nickname) {
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator users_it = users.begin();

	for (; users_it != users.end(); users_it++) {
		if (users_it->second.getNickname() == nickname)
			return true;
	}
	return false;
}

bool checkUserInvited(Server *server, std::string &channel_name, std::string &nickname) {
	Channel &channel = server->getChannels()[channel_name];
	std::vector<std::string> &invited_users = channel.getInvitedUsers();

	for (size_t i = 0; i < invited_users.size(); i++) {
		if (invited_users[i] == nickname)
			return true;
	}
	return false;
}

std::string formatTime(std::time_t raw) {
	std::tm* time = std::localtime(&raw);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time);
	return std::string(buffer);
}

std::string getTarget(std::string &params) {
	size_t pos = params.find(' ');
	std::string target = params.substr(0, pos);
	return target;
}
