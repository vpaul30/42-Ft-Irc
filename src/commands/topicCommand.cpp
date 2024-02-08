#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"
#include "../../includes/channel.hpp"

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

bool Server::channelNotExistent(const std::string& channel) {
	std::map<std::string, Channel>::iterator iter = m_channels.find(channel);
	return iter == m_channels.end();
}

bool Server::userInChannel(const User& user, const std::string& channel) {
	if (m_channels.find(channel) == m_channels.end())
		return false;
	Channel& channelObj = m_channels[channel];
	const std::vector<User>& users = channelObj.getUsers();
	for (size_t i = 0; i < users.size(); ++i) {
		if (users[i].getNickname() == user.getNickname())
			return true;
	}
	return false;
}

bool Server::userIsOperator(const User& user, const std::string& channel) {
	if (m_channels.find(channel) == m_channels.end())
		return false;
	Channel& channelObj = m_channels[channel];
	const std::vector<User>& operators = channelObj.getOperators();
	for (size_t i = 0; i < operators.size(); ++i) {
		if (operators[i].getNickname() == user.getNickname())
			return true;
	}
	return false;
}

std::string formatTime(std::time_t raw) {
	std::tm* time = std::localtime(&raw);
	char buffer[80];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", time);
	return std::string(buffer);
}

int Server::topicCommand(User &user, MsgInfo &msg_info) {

	std::string reply;
	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
		return 0;
	}

	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string channelName = splitParams.first;
	std::string topic = splitParams.second;
	Channel& channelObj = m_channels[channelName];
	
	if (channelNotExistent(channelName)) {
		// ERR_NOSUCHCHANNEL (403)
		reply = ERR_NOSUCHCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (!userInChannel(user, channelName)) {
		// ERR_NOTONCHANNEL (442)
		reply = ERR_NOTONCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (topic.empty()) {
		if (channelObj.getTopic().empty()) {
			// RPL_NOTOPIC (331)
			reply = RPL_NOTOPIC(user.getNickname(), channelName);
			addRplAndPollout(user, reply);
			return 0;
		} else {
			// RPL_TOPIC (332)
			reply = RPL_TOPIC(user.getNickname(), channelName, channelObj.getTopic());
			addRplAndPollout(user, reply);
			// RPL_TOPICWHOTIME (333)
			reply = RPL_TOPICWHOTIME(user.getNickname(), channelName, channelObj.getTopicSetter(), formatTime(channelObj.getTimeOfTopic()));
			addRplAndPollout(user, reply);
			return 0;
		}
	} else {
		if (!userIsOperator(user, channelName)) {
			// ERR_CHANOPRIVSNEEDED (482)
			reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channelName);
			addRplAndPollout(user, reply);
			return 0;
		}

		channelObj.setTopic(topic, user.getNickname());
		//broadcast message to all users in channel?

		// RPL_TOPIC (332)
		reply = RPL_TOPIC(user.getNickname(), channelName, channelObj.getTopic());
		addRplAndPollout(user, reply);
		// RPL_TOPICWHOTIME (333)
		reply = RPL_TOPICWHOTIME(user.getNickname(), channelName, channelObj.getTopicSetter(), formatTime(channelObj.getTimeOfTopic()));
		addRplAndPollout(user, reply);
	}
	return 0;
}
