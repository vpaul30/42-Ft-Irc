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

		ERR_NOSUCHNICK (401)	--> manually added

	Command Example:
		INVITE Wiz #foo_bar    ; Invite Wiz to #foo_bar

		:dan-!d@localhost INVITE Wiz #test    ; dan- has invited Wiz to the channel #test
*/

// implement functions for: userExists, nickInChannel

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

bool Server::nickNotExistent(const std::string& nickname) {
	for (std::map<int, User>::iterator it = m_users.begin(); it != m_users.end(); ++it) {
		if (it->second.getNickname() == nickname)
			return false;
	}
	return true;
}

bool Server::nickInChannel(const User& nick, const std::string& channel) {
	const std::vector<User>& users = m_channels[channel].getUsers();
	for (size_t i = 0; i < users.size(); ++i) {
		if (users[i].getNickname() == nick.getNickname())
			return true;
	}
	return false;
}

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

	if (!userIsOperator(user, channelName)) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (!userInChannel(user, channelName)) {
		// ERR_USERONCHANNEL (443)
		std::string reply = ERR_USERONCHANNEL(user.getNickname(), nickname, channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (nickNotExistent(nickname)) {
		// ERR_NOSUCHNICK (401)
		std::string reply = ERR_NOSUCHNICK(user.getNickname(), nickname);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (channelNotExistent(channelName)) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (!nickInChannel(user, channelName)) {
		// ERR_NOTONCHANNEL (442)
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	//invite nickname to channel --> implement logic
	//privmsg nickname, that he has been invited
	//broadcast message to all users in channel?

	// RPL_INVITING (341)
	std::string reply = RPL_INVITING(user.getNickname(), nickname, channelName);
	addRplAndPollout(user, reply);

	return 0;
}
