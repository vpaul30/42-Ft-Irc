#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: PRIVMSG
  	Parameters: <target> <text to be sent>

	Numeric Replies:
		ERR_NOSUCHNICK (401)
		ERR_NOSUCHSERVER (402)
		ERR_CANNOTSENDTOCHAN (404)
		ERR_TOOMANYTARGETS (407)
		ERR_NORECIPIENT (411)
		ERR_NOTEXTTOSEND (412)
		ERR_NOTOPLEVEL (413)
		ERR_WILDTOPLEVEL (414)
		RPL_AWAY (301)

	Command Example:
  		PRIVMSG Tony :Hey how are you? --> sends private message to Tony "Hey how are you?"
		PRIVMSG Tony Hey how are you? --> sends private message to Tony "Hey"
		PRIVMSG #myChannel :Hey guys --> sends message to everyone on channel #myChannel "Hey guys"

	PRIVMSG --> :localhost 411 <nickname> :No recipient given (PRIVMSG)
	PRIVMSG Tony --> :localhost 412 <nickname> :No text to send
	PRIVMSG <invalid nickname> text --> :localhost 401 <nickname> <invalid nickname> :No such nick/channel
*/

static bool hasOneWord(std::string &params);
static std::string getTarget(std::string &params);
static bool checkTargetExist(Server *server, std::string &params);
static std::string getMessageText(std::string &params);


int Server::privmsgCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NORECIPIENT (411)
		std::string reply = ERR_NORECIPIENT(user.getNickname());
		addRplAndPollout(user, reply);
	}
	else if (hasOneWord(msg_info.params) == true) { // only target, no text
		// ERR_NOTEXTTOSEND (412)
		std::string reply = ERR_NOTEXTTOSEND(user.getNickname());
		addRplAndPollout(user, reply);
	} 
	else if (checkTargetExist(this, msg_info.params) == false) {
		// ERR_NOSUCHNICK (401)
		std::string target = getTarget(msg_info.params);
		std::string reply = ERR_NOSUCHNICK(user.getNickname(), target);
		addRplAndPollout(user, reply);
	} else {
		std::string target = getTarget(msg_info.params);
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += PRIVMSG(target, getMessageText(msg_info.params));
		User *target_user = getUserByNickname(target);
		// if (target_user == NULL) error
		addRplAndPollout(*target_user, reply);
	}
	return 0;
}

static bool hasOneWord(std::string &params) {
	if (params.find(' ') != std::string::npos)
		return false;
	return true;
}

static std::string getTarget(std::string &params) {
	size_t pos = params.find(' ');
	std::string target = params.substr(0, pos);
	return target;
}

static bool checkTargetExist(Server *server, std::string &params) {
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator it = users.begin();
	std::string target = getTarget(params);

	for (; it != users.end(); it++) {
		if (it->second.getNickname() == target)
			return true;
	}
	return false;
}

static std::string getMessageText(std::string &params) {
	std::string copy = params;
	std::string text;
	size_t pos;

	pos = copy.find(' ');
	copy.erase(0, pos); // erase 1st word (target)
	pos = copy.find_first_not_of(' '); 
	copy.erase(0, pos); // erase spaces after 1st word (target)

	if (copy[0] == ':') {
		copy.erase(0, 1); // erase ':'
		text = copy;
	} else {
		if ((pos = copy.find(' ')) != std::string::npos)
			text = copy.substr(0, pos);
		else
			text = copy;
	}
	return text;
}