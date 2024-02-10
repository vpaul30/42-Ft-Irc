#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*

	Command: PART
  	Parameters: <channel>{,<channel>} [<reason>]

	Numeric Replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_NOSUCHCHANNEL (403)
		ERR_NOTONCHANNEL (442)

	Command Examples:
		PART #twilight_zone  -->  leave channel "#twilight_zone"
*/

int Server::partCommand(User &user, MsgInfo msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
		return 0;
	}

	std::string channel_name = getTarget(msg_info.params);
	if (checkChannelExist(this, channel_name) == false) {
		// ERR_NOSUCHCHANNEL (403)
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channel_name);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (checkUserInChannel(this, channel_name, user.getNickname()) == false) {
		// ERR_NOTONCHANNEL (442)
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channel_name);
		addRplAndPollout(user, reply);
		return 0;
	}

	// leave channel (erase user's nickname from channel)
	std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
	reply += PART(channel_name);
	addRplAndPollout(user, reply);
	m_channels[channel_name].broadcastMsg(this, user.getNickname(), reply);
	removeUserFromChannel(this, channel_name, user.getNickname());
	return 0;
}
