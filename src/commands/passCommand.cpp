#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: PASS
	Parameters: <password>

	Numeric replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_ALREADYREGISTERED (462)
		ERR_PASSWDMISMATCH (464)
	
	Command Example:
		PASS secretpasswordhere
*/

int Server::passCommand(User &user, MsgInfo &msg_info) {

	if (user.getIsAuthorised() == true) {
		// ERR_ALREADYREGISTERED (462)
		std::string reply = ERR_ALREADYREGISTERED(user.getNickname());
		addRplAndPollout(user, reply);

	} else if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);

	} else if (msg_info.params != m_password) {
		// ERR_PASSWDMISMATCH (464)
		std::string reply = ERR_PASSWDMISMATCH(user.getNickname());
		addRplAndPollout(user, reply);

	} else {
		// correct password
		user.setIsPassValid(true);
	}
	return 0;
}