#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: NICK
  	Parameters: <nickname>

	Numeric Replies:
		ERR_NONICKNAMEGIVEN (431)
		ERR_ERRONEUSNICKNAME (432)
		ERR_NICKNAMEINUSE (433)

	Command Example:
  		NICK Wiz
		(if user is already registered server will reply on successful nick change)
*/

bool validateNickname(const std::string &nickname);
bool isNicknameAvailable(Server *server, const std::string &nickname);


int Server::nickCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NONICKNAMEGIVEN (431)
		std::string reply = ERR_NONICKNAMEGIVEN(user.getNickname());
		addRplAndPollout(user, reply);
		return 0;
	}
	if (validateNickname(msg_info.params) == false) {
		// ERR_ERRONEUSNICKNAME (432)
		std::string reply = ERR_ERRONEUSNICKNAME(user.getNickname(), msg_info.params);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (isNicknameAvailable(this, msg_info.params) == false) {
		// ERR_NICKNAMEINUSE (433)
		std::string reply = ERR_NICKNAMEINUSE(user.getNickname(), msg_info.params);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (user.getIsAuthorised() == true) {
		std::string old_nickname = user.getNickname();
		user.setNickname(msg_info.params);
		std::string reply = prefix(old_nickname, user.getUsername(), user.getHostname());
		reply += NICK(old_nickname, user.getUsername(), user.getHostname(), msg_info.params);
		addRplAndPollout(user, reply);

	} else {
		user.setNickname(msg_info.params);
	}

	return 0;
}

bool validateNickname(const std::string &nickname) {
	// no leading '#' or ':'
	// no ASCII space
	// no ',' '.' '*' '?' '!' '@'
	if (!std::isalpha(nickname[0]))
		return false;
	if (nickname.find(" \f\n\r\t\v,.*?!@") != std::string::npos)
		return false;
	return true;
}

bool isNicknameAvailable(Server *server, const std::string &nickname) {
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator it = users.begin();

	for (; it != users.end(); it++) {
		if (it->second.getNickname() == nickname)
			return false;
	}

	return true;
}

