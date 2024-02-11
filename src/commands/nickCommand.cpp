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

static bool validateNickname(const std::string &nickname);
static bool isNicknameAvailable(Server *server, const std::string &nickname);
static void replaceNicknameInChannels(Server *server, std::string &old_nickname, std::string &new_nickname);

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
		replaceNicknameInChannels(this, old_nickname, user.getNickname());
	} else {
		std::string old_nickname = user.getNickname();
		user.setNickname(msg_info.params);
	}

	return 0;
}

static bool validateNickname(const std::string &nickname) {
	// no leading '#' or ':'
	// no ASCII space
	// no ',' '.' '*' '?' '!' '@'
	if (!std::isalpha(nickname[0]))
		return false;
	if (nickname.find(" \f\n\r\t\v,.*?!@") != std::string::npos)
		return false;
	return true;
}

static bool isNicknameAvailable(Server *server, const std::string &nickname) {
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator it = users.begin();
	if (nickname == "Bot3000")
		return false;
	for (; it != users.end(); it++) {
		if (it->second.getNickname() == nickname)
			return false;
	}

	return true;
}

static void replaceNicknameInChannels(Server *server, std::string &old_nickname, std::string &new_nickname) {
	std::map<std::string, Channel> &channels = server->getChannels();
	std::map<std::string, Channel>::iterator channels_it = channels.begin();

	std::vector<std::string>::iterator nicknames_it;
	for (; channels_it != channels.end(); channels_it++) {
		nicknames_it = channels_it->second.getUsers().begin();
		for (; nicknames_it != channels_it->second.getUsers().end(); nicknames_it++) {
			if (*nicknames_it == old_nickname)
				*nicknames_it = new_nickname;
		}
		nicknames_it = channels_it->second.getOperators().begin();
		for (; nicknames_it != channels_it->second.getOperators().end(); nicknames_it++) {
			if (*nicknames_it == old_nickname)
				*nicknames_it = new_nickname;
		}
	}
}
