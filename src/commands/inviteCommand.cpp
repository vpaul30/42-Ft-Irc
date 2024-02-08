#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: INVITE
	Parameters: <nickname> <channel>

	Numeric replies:
		RPL_INVITING (341)
		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHCHANNEL (403)
		ERR_NOTONCHANNEL (442)
		ERR_CHANOPRIVSNEEDED (482)
		ERR_USERONCHANNEL (443)

		ERR_NOSUCHNICK (401)	--> manually added

	Command Example:
		INVITE Wiz #foo_bar    ; Invite Wiz to #foo_bar

		:dan-!d@localhost INVITE Wiz #test    ; dan- has invited Wiz to the channel #test
*/

// implement functions for: userExists, channelExists, userInChannel, nickInChannel, userIsOperator

static std::pair<std::string, std::string> paramSplit(const std::string& params) {
	size_t firstSpace = params.find(' ');
	if (firstSpace != std::string::npos) {
		std::string firstPart = params.substr(0, firstSpace);
		std::string secondPart = params.substr(firstSpace + 1);
		return std::make_pair(firstPart, secondPart);
	} else {
		return std::make_pair(params, "");
	}
}

int Server::inviteCommand(User &user, MsgInfo &msg_info) {

	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
	}

	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string nickname = splitParams.first;
	std::string channel = splitParams.second;

	if (!userExists(nickname)) {
		// ERR_NOSUCHNICK (401)
		std::string reply = ERR_NOSUCHNICK(user.getNickname(), nickname);
		addRplAndPollout(user, reply);
	}

	if (!channelExists(channel)) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channel);
		addRplAndPollout(user, reply);
	}
	
	if (!userInChannel(user, channel)) {
		// ERR_NOTONCHANNEL (442)
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channel);
		addRplAndPollout(user, reply);
	}
	
	if (nickInChannel(nickname, channel)) {
		// ERR_USERONCHANNEL (443)
		std::string reply = ERR_USERONCHANNEL(user.getNickname(), nickname, channel);
		addRplAndPollout(user, reply);
	}

	if (!userIsOperator(user, channel)) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channel);
		addRplAndPollout(user, reply);
	}
	//invite nickname to channel --> implement logic
	//privmsg nickname, that he has been invited
	//broadcast message to all users in channel?

	// RPL_INVITING (341)
	std::string reply = RPL_INVITING(user.getNickname(), nickname, channel);
	addRplAndPollout(user, reply);

	return 0;
}
