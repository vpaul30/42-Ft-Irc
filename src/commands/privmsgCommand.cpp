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
// static std::string getTarget(std::string &params);
static bool checkTargetExist(Server *server, std::string &params);
static std::string getMessageText(std::string &params);
// static bool checkUserInChannel(Server *server, std::string &nickname, std::string &channel_name);


int Server::privmsgCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NORECIPIENT (411)
		std::string reply = ERR_NORECIPIENT(user.getNickname());
		addRplAndPollout(user, reply);
		return 0;
	}
	if (hasOneWord(msg_info.params) == true) { // only target, no text
		// ERR_NOTEXTTOSEND (412)
		std::string reply = ERR_NOTEXTTOSEND(user.getNickname());
		addRplAndPollout(user, reply);
		return 0;
	} 
	if (checkTargetExist(this, msg_info.params) == false) {
		// ERR_NOSUCHNICK (401)
		std::string target = getTarget(msg_info.params);
		std::string reply = ERR_NOSUCHNICK(user.getNickname(), target);
		addRplAndPollout(user, reply);
		return 0;
	}
	std::string target = getTarget(msg_info.params);
	if (target[0] == '#') { // target is a channel
		if (checkUserInChannel(this, target, user.getNickname()) == false) {
			// ERR_CANNOTSENDTOCHAN (404)
			std::string reply = ERR_CANNOTSENDTOCHAN(user.getNickname(), target);
			addRplAndPollout(user, reply);
			return 0;
		}
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += PRIVMSG(target, getMessageText(msg_info.params));
		m_channels[target].broadcastMsg(this, user.getNickname(), reply);
	} else { // target is another user
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += PRIVMSG(target, getMessageText(msg_info.params));
		User *target_user = getUserByNickname(target);
		if (target_user == NULL) {
			errorMsg("getUserByNickname() error.");
			return 1;
		}
		addRplAndPollout(*target_user, reply);
	}
	return 0;
}

static bool hasOneWord(std::string &params) {
	if (params.find(' ') != std::string::npos)
		return false;
	return true;
}

// static std::string getTarget(std::string &params) {
// 	size_t pos = params.find(' ');
// 	std::string target = params.substr(0, pos);
// 	return target;
// }

static bool checkTargetExist(Server *server, std::string &params) {
	std::string target = getTarget(params);

	// check users
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator users_it = users.begin();
	for (; users_it != users.end(); users_it++) {
		if (users_it->second.getNickname() == target)
			return true;
	}
	//check channels
	std::map<std::string, Channel> &channels = server->getChannels();
	std::map<std::string, Channel>::iterator channels_it = channels.begin();
	for (; channels_it != channels.end(); channels_it++) {
		if (channels_it->first == target)
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
