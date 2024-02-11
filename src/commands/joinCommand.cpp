#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"
#include "../../includes/utils.hpp"

/*
	Command: JOIN
  	Parameters: <channel>{,<channel>} [<key>{,<key>}]

	Numeric Replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHCHANNEL (403)
		ERR_TOOMANYCHANNELS (405)
		ERR_BADCHANNELKEY (475)
		ERR_BANNEDFROMCHAN (474)
		ERR_CHANNELISFULL (471)
		ERR_INVITEONLYCHAN (473)
		ERR_BADCHANMASK (476)
		RPL_TOPIC (332)
		RPL_TOPICWHOTIME (333)
		RPL_NAMREPLY (353)
		RPL_ENDOFNAMES (366)

	Command Example: 
		JOIN #testChannel
		JOIN #channelWithPassword 123
		JOIN #channel1,#channel2
*/

static void getChannelNames(std::vector<std::string> &channel_names, std::string &params);
static void getChannelKeys(std::vector<std::string> &channel_keys, std::string &params);
static bool isChannelNameValid(std::string &channel_name);
static void removeUserFromInvited(Server *server, std::string &channel_name, std::string &nickname);

int Server::joinCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), "JOIN");
		addRplAndPollout(user, reply);
		return 0;
	}

	std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
	std::string channel_name = splitParams.first;
	std::string key = splitParams.second;
	size_t pos;
	if ((pos = key.find(' ')) != std::string::npos) {
		key = key.substr(0, pos);
	}

	if (isChannelNameValid(channel_name) == false) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channel_name);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkChannelExist(this, channel_name) == true) {
		if (checkUserInChannel(this, channel_name, user.getNickname()) == true)
			return 0;
		Channel &channel_to_join = m_channels[channel_name];
		if (!channel_to_join.getPassword().empty()) {
			if (key.empty() || key != channel_to_join.getPassword()) {
				std::string reply = ERR_BADCHANNELKEY(user.getNickname(), channel_name);
				addRplAndPollout(user, reply);
				return 0;
			}
		} else if (channel_to_join.getInviteOnly() == true) {
			if (checkUserInvited(this, channel_name, user.getNickname()) == false)
			{
				std::string reply = ERR_INVITEONLYCHAN(user.getNickname(), channel_name); // remove from invites when joins
				addRplAndPollout(user, reply);
				return 0;
			}
		} else if (channel_to_join.getUsersLimit() != -1) {
			if (channel_to_join.getUsers().size() + channel_to_join.getOperators().size() >= channel_to_join.getUsersLimit()) {
				std::string reply = ERR_CHANNELISFULL(user.getNickname(), channel_name);
				addRplAndPollout(user, reply);
				return 0;
			}
		}
		channel_to_join.addNewUser(user.getNickname());
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += JOIN(channel_name);
		addRplAndPollout(user, reply);
		channel_to_join.broadcastMsg(this, user.getNickname(), reply);
		if (channel_to_join.getTopic().empty() == false) {
			reply = RPL_TOPIC(user.getNickname(), channel_name, channel_to_join.getTopic());
			reply += RPL_TOPICWHOTIME(user.getNickname(), channel_name,
					channel_to_join.getTopicSetter(), formatTime(channel_to_join.getTimeOfTopic()));
			addRplAndPollout(user, reply);
		}
		if (channel_to_join.getInviteOnly() == true) // remove user from invited list
			removeUserFromInvited(this, channel_name, user.getNickname());
	} else { // channel doesn't exist, create a new one
		Channel new_channel(channel_name, user.getNickname());
		m_channels.insert(std::pair<std::string, Channel>(channel_name, new_channel));
		std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
		reply += JOIN(channel_name);
		addRplAndPollout(user, reply);
	}
	return 0;
}

static bool isChannelNameValid(std::string &channel_name) {
	if (channel_name[0] != '#')
		return false;
	if (channel_name.size() == 1)
		return false;
	return true;
}

static void removeUserFromInvited(Server *server, std::string &channel_name, std::string &nickname) {
	Channel &channel = server->getChannels()[channel_name];

	std::vector<std::string>::iterator nicknames_it = channel.getInvitedUsers().begin();
	for (; nicknames_it != channel.getInvitedUsers().end(); nicknames_it++) {
		if (*nicknames_it == nickname) {
			channel.getInvitedUsers().erase(nicknames_it);
			break;
		}
	}	
}

