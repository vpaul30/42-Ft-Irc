#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"
#include "../../includes/channel.hpp"

/*
	Command: KICK
	Parameters: <channel> <user> *( "," <user> ) [<comment>]

	Numeric replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHCHANNEL (403)
		ERR_CHANOPRIVSNEEDED (482)
		ERR_USERNOTINCHANNEL (441)
		ERR_NOTONCHANNEL (442)
		ERR_NOSUCHNICK (401)


	Command Example:
		KICK #Finnish Matthew           ; Command to kick Matthew from #Finnish

		KICK &Melbourne Matthew         ; Command to kick Matthew from &Melbourne

		KICK #Finnish John :Speaking English         ; Command to kick John from #Finnish using "Speaking English" as the reason (comment).

		:WiZ!jto@tolsun.oulu.fi KICK #Finnish John        ; KICK message on channel #Finnish from WiZ to remove John from channel
*/

// static std::pair<std::string, std::string> paramSplit(const std::string& params);
static std::vector<std::string> splitUsersList(const std::string& usersList, char delimiter);

int Server::kickCommand(User &user, MsgInfo &msg_info) {
	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string channelName = splitParams.first;
	std::string remainder = splitParams.second;

	if (msg_info.params.empty() || channelName.empty() || remainder.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkChannelExist(this, channelName) == false) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkUserInChannel(this, channelName, user.getNickname()) == false) {
		// ERR_NOTONCHANNEL (442)
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (!checkUserChannelOperator(this, channelName, user.getNickname()) && !user.getIsOperator()) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	size_t spacePos = remainder.find(' ');
	std::string usersList = (spacePos != std::string::npos) ? remainder.substr(0, spacePos) : remainder;
	std::string comment = (spacePos != std::string::npos) ? remainder.substr(spacePos + 1) : "";
	if (comment[0] == ':') {
		comment.erase(0, 1);
	} else {
		comment = comment.substr(0, comment.find(' '));
	}

	std::vector<std::string> users = splitUsersList(usersList, ',');
	for (size_t i = 0; i < users.size(); ++i) {
		if (checkUserExist(this, users[i]) == false) {
			// ERR_NOSUCHNICK (401)
			std::string reply = ERR_NOSUCHNICK(user.getNickname(), users[i]);
			addRplAndPollout(user, reply);
			return 0;
		}
		if (checkUserInChannel(this, channelName, users[i]) == false) {
			// ERR_USERNOTINCHANNEL (441)
			std::string reply = ERR_USERNOTINCHANNEL(user.getNickname(), users[i], channelName);
			addRplAndPollout(user, reply);
		}
		if (comment.empty())
			comment = users[i];
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += KICK(channelName, users[i], comment);
		addRplAndPollout(user, reply);
		m_channels[channelName].broadcastMsg(this, user.getNickname(), reply);
		removeUserFromChannel(this, channelName, users[i]);
	}
	return 0;
}

// static std::pair<std::string, std::string> paramSplit(const std::string& params) {
// 	size_t firstSpace = params.find(' ');
// 	if (firstSpace != std::string::npos) {
// 		std::string firstPart = params.substr(0, firstSpace);
// 		std::string secondPart = params.substr(firstSpace + 1);
// 		return std::make_pair(firstPart, secondPart);
// 	} else {
// 		return std::make_pair(params, "");
// 	}
// }

static std::vector<std::string> splitUsersList(const std::string& usersList, char delimiter) {
	std::vector<std::string> result;
	std::string::size_type start = 0;
	std::string::size_type end = usersList.find(delimiter);

	while (end != std::string::npos) {
		result.push_back(usersList.substr(start, end - start));
		start = end + 1;
		end = usersList.find(delimiter, start);
	}
	result.push_back(usersList.substr(start));

	return result;
}
