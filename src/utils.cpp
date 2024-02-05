#include "../includes/utils.hpp"

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

