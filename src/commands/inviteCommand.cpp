#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"
#include "../../includes/channel.hpp"

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
		ERR_NOSUCHNICK (401)

	Command Example:
		INVITE Wiz #foo_bar    ; Invite Wiz to #foo_bar

		:dan-!d@localhost INVITE Wiz #test    ; dan- has invited Wiz to the channel #test
*/

// static std::pair<std::string, std::string> paramSplit(std::string& params);

int Server::inviteCommand(User &user, MsgInfo &msg_info) {
	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string nickname = splitParams.first;
	std::string channelName = splitParams.second;

	if (msg_info.params.empty() || nickname.empty() || channelName.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkUserInChannel(this, channelName, user.getNickname()) == false) {
		// ERR_NOTONCHANNEL (442)
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkUserExist(this, nickname) == false) {
		// ERR_NOSUCHNICK (401)
		std::string reply = ERR_NOSUCHNICK(user.getNickname(), nickname);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkChannelExist(this, channelName) == false) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkUserInChannel(this, channelName, nickname) == true) {
		// ERR_USERONCHANNEL (443)
		std::string reply = ERR_USERONCHANNEL(user.getNickname(), nickname, channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (!checkUserChannelOperator(this, channelName, user.getNickname()) && !user.getIsOperator()) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	// RPL_INVITING (341)
	std::string reply = RPL_INVITING(user.getNickname(), nickname, channelName);
	addRplAndPollout(user, reply);

	if (checkUserInvited(this, channelName, nickname) == false)
		m_channels[channelName].addInvitedUser(nickname);

	User *user_to_invite = getUserByNickname(nickname);
	if (user_to_invite == NULL) {
		errorMsg("getUserByNickname() returned NULL");
		return 0;
	}
	reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
	reply += INVITE(nickname, channelName);
	addRplAndPollout(*user_to_invite, reply);
	return 0;
}

// static std::pair<std::string, std::string> paramSplit(std::string& params) {
// 	size_t firstSpace = params.find(' ');
// 	if (firstSpace != std::string::npos) {
// 		std::string firstPart = params.substr(0, firstSpace);
// 		std::string secondPart = params.substr(firstSpace + 1);
// 		return std::make_pair(firstPart, secondPart);
// 	} else {
// 		return std::make_pair(params, "");
// 	}
// }
