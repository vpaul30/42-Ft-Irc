#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: TOPIC
	Parameters: <channel> [<topic>]

	Numeric replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHCHANNEL (403)
		ERR_NOTONCHANNEL (442)
		ERR_CHANOPRIVSNEEDED (482)
		RPL_NOTOPIC (331)
		RPL_TOPIC (332)
		RPL_TOPICWHOTIME (333)

	Command Example:
		TOPIC #test :New topic          ; Setting the topic on "#test" to "New topic".

		TOPIC #test :                   ; Clearing the topic on "#test"

		TOPIC #test                     ; Checking the topic for "#test"
*/

// invite only channels?
// implement channel.getcurrenttopic, channel.topicsetter, channel.timeoftopic


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

static bool channelExists(const std::string& channel) {
	return m_channels.find(channel) != m_channels.end();
}

static bool userInChannel(const User& user, const std::string& channel) {
	if (m_channels.find(channel) == m_channels.end())
		return false;
	Channel& channel = m_channels[channel];
	const std::vector<User>& users = channel.getUsers();
	for (size_t i = 0; i < users.size(); ++i) {
		if (users[i].nickname == user.nickname)
			return true;
	}
	return false;
}

static bool userIsOperator(const User& user, const std::string& channel) {
	if (m_channels.find(channel) == m_channels.end())
		return false;
	Channel& channel = m_channels[channel];
	if (!channel.getUsers().empty() && channel.getUsers()[0].nickname == user.nickname)
		return true;
	return false;
}

static bool topicIsSet(const std::string& channel){
	if (m_channels.find(channel) == m_channels.end())
		return false;
	return !m_channels[channel].topic.empty();
}

int Server::topicCommand(User &user, MsgInfo &msg_info) {

	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
	}

	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string channel = splitParams.first;
	std::string topic = splitParams.second;
	
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

	if (channel && !topic) {
		if (!topicIsSet()) {
			// RPL_NOTOPIC (331)
			std::string reply = RPL_NOTOPIC(user.getNickname(), channel);
			addRplAndPollout(user, reply);
		}
		// RPL_TOPIC (332)
		std::string reply = RPL_TOPIC(user.getNickname(), channel, //current_topic?);
		addRplAndPollout(user, reply);
	} else {
		if (!userIsOperator(user, channel)) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channel);
		addRplAndPollout(user, reply);
		}

		//set topic to channel --> implement logic
		//broadcast message to all users in channel?

		// RPL_TOPIC (332)
		std::string reply = RPL_TOPIC(user.getNickname(), channel, //current_topic?);
		addRplAndPollout(user, reply); --> no pollout because of TOPICWHOTIME
		// RPL_TOPICWHOTIME (333)
		std::string reply = RPL_TOPICWHOTIME(user.getNickname(), channel, //who, //time);
		addRplAndPollout(user, reply);
	}
	return 0;
}
