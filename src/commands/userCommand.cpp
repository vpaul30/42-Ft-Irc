#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: USER
	Parameters: <username> 0 * <realname>

	Numeric Replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_ALREADYREGISTERED (462)
	
	Command Examples:
		USER guest 0 * :Ronnie Reagan  -->  "guest" is stored as username, "Ronnie Reagan" is stored as realname
		USER guest 0 * Ronnie Reagan  -->  "guest" is stored as username, "Ronnie" is stored as realname

*/

static bool validateParams(std::string &params);
static void assignUsernameAndRealname(User &user, std::string &params);

int Server::userCommand(User &user, MsgInfo &msg_info) {
	if (user.getIsAuthorised() == true) {
		// ERR_ALREADYREGISTERED (462)
		std::string reply = ERR_ALREADYREGISTERED(user.getNickname());
		addRplAndPollout(user, reply);

	} else if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);

	} else if (validateParams(msg_info.params) == false) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);

	}
	assignUsernameAndRealname(user, msg_info.params);
	return 0;
}

// should be at least 4 words
static bool validateParams(std::string &params) {
	std::string copy = params;
	size_t pos;
	int word_counter = 0;

	while (word_counter < 4) {
		if ((pos = copy.find(' ')) != std::string::npos) {
			word_counter++;
			copy.erase(0, pos); // erase word
			copy.erase(0, copy.find_first_not_of(' ')); // erase spaces until a new word	
		} else { // pos == std::string::npos
			word_counter++;
			break;
		}
	}
	if (word_counter < 4)
		return false;
	return true;
}

// Assign 1st word to user.m_username
// skip next 2 words
// if 4th word starts with ':' --> assign everything after ':' to user.m_realname
// else --> assign only 4th word to user.m_realname
static void assignUsernameAndRealname(User &user, std::string &params) {
	size_t pos;
	pos = params.find(' '); // find space after word
	std::string username = params.substr(0, pos);
	user.setUsername(username);
	params.erase(0, pos); // erase word
	params.erase(0, params.find_first_not_of(' ')); // erase spaces until a new word	
	// skip 2 words
	for (int i = 0; i < 2; i++) {
		pos = params.find(' '); // find space after word
		params.erase(0, pos); // erase word
		params.erase(0, params.find_first_not_of(' ')); // erase spaces until a new word	
	}
	if (params[0] == ':') {
		params.erase(0, 1); // erase ':'
		user.setRealname(params);
	} else if ((pos = params.find(' ')) != std::string::npos) {
		std::string realname = params.substr(0, pos);
		user.setRealname(realname);
	} else { // pos == std::string::npos
		user.setRealname(params);
	}
}
