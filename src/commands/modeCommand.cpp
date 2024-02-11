#include "../../includes/channel.hpp"
#include "../../includes/replies.hpp"
#include "../../includes/server.hpp"

/*
    Command: MODE
    Parameters: <channel> {[+|-]|i|t|k|o|l} [<limit>] [<user>]

    The various modes available for channels are as follows:
        i - invite-only channel flag;
        t - topic settable by channel operator only flag;
        o - give/take channel operator privileges;
        k - set a channel key (password);
        l - set the user limit to channel;

    Numeric replies:
        RPL_CHANNELMODEIS (324)
        ERR_NEEDMOREPARAMS (461)
        ERR_CHANOPRIVSNEEDED (482)
        ERR_NOSUCHNICK (401)
        ERR_NOTONCHANNEL (442) // me
		ERR_USERNOTINCHANNEL (441) // target
        ERR_NOSUCHCHANNEL (403)
        ERR_UNKNOWNMODE (472)
        ERR_KEYSET (467)

    Command Example:
        MODE #Finnish +im               ; Makes #Finnish channel moderated and 'invite-only'.

        MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on channel #Finnish.

        MODE #42 +k oulu                ; Set the channel key to "oulu".

        MODE #eu-opers +l 10            ; Set the limit for the number of users on channel to 10.
*/

static bool checkModeValid(std::string &mode);
static bool checkModeExist(char mode);

int Server::modeCommand(User &user, MsgInfo &msg_info)
{
    std::pair<std::string, std::string> split_params = paramSplit(msg_info.params);
    std::string channel_name = split_params.first;
    std::string mode_n_params = split_params.second;

    if (channel_name.empty()) {
        // ERR_NEEDMOREPARAMS (461)
        std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
        addRplAndPollout(user, reply);
        return 0;
    }

    std::pair<std::string, std::string> split_mode_n_params = paramSplit(mode_n_params);
	std::string mode = split_mode_n_params.first;
	std::string mode_params = split_mode_n_params.second;
	size_t pos;
	if ((pos = mode_params.find(' ')) != std::string::npos) {
		mode_params = mode_params.substr(0, pos);
	}

	// std::cout << channel_name << "," << mode << "," << mode_params << std::endl;

	if (checkChannelExist(this, channel_name) == false) {
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
	Channel &channel = m_channels[channel_name];
	if (mode_n_params.empty()) {
		std::string channel_modes;
		if (!channel.getPassword().empty())
			channel_modes += 'k';
		if (channel.getInviteOnly())
			channel_modes += 'i';
		if (channel.getTopicRestriction())
			channel_modes += 't';
		if (channel.getUsersLimit() > 0)
			channel_modes += 'l';
		std::string reply = RPL_CHANNELMODEIS(user.getNickname(), channel_name, channel_modes);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (checkModeValid(mode) == false) {
		// ERR_UNKNOWNMODE (472)
		std::string reply = ERR_UNKNOWNMODE(user.getNickname(), mode);
		addRplAndPollout(user, reply);
		return 0;
	}
	if (!checkUserChannelOperator(this, channel_name, user.getNickname()) && !user.getIsOperator()) {
		// ERR_CHANOPRIVSNEEDED (482)
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channel_name);
		addRplAndPollout(user, reply);
		return 0;
	}

	char mode_action = mode.size() == 1 ? '+' : mode[0];
	char mode_char = mode.size() == 1 ? mode[0] : mode[1];

	switch (mode_char) {
	case 'i':
		if (mode_action == '+') {
			if (channel.getInviteOnly() == true)
				return 0; // do nothing
			channel.setInviteOnly(true);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, "");
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		} else {
			if (channel.getInviteOnly() == false)
				return 0; // do nothing
			channel.setInviteOnly(false);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, "");
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		break;
	case 't':
		if (mode_action == '+') {
			if (channel.getTopicRestriction() == true)
				return 0; // do nothing
			channel.setTopicRestriction(true);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, "");
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		} else {
			if (channel.getTopicRestriction() == false)
				return 0; // do nothing
			channel.setTopicRestriction(false);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, "");
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		break;
	case 'o':
		if (mode_params.empty()) { // no target nickname
	        // ERR_NEEDMOREPARAMS (461)
			std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
			addRplAndPollout(user, reply);
			return 0;
		}
		// if (!mode_params.empty() && checkUserInChannel(this, channel_name, mode_params)) {
		if (checkUserExist(this, mode_params) == false) {
			// ERR_NOSUCHNICK (401)
			std::string reply = ERR_NOSUCHNICK(user.getNickname(), mode_params);
			addRplAndPollout(user, reply);
			return 0;
		}
		if (checkUserInChannel(this, channel_name, mode_params) == false) {
			// ERR_USERNOTINCHANNEL (441)
			std::string reply = ERR_USERNOTINCHANNEL(user.getNickname(), mode_params, channel_name);
			addRplAndPollout(user, reply);
			return 0;
		}
		if (mode_action == '+') {
			// bool isAdded = channel.addMode(mode[i]); // ADD and REMOVE modes
			removeUserFromChannel(this, channel_name, mode_params);
			channel.addNewOperator(mode_params);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, mode_params);
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		else {
			// bool isRemoved = channel.removeMode(mode[i]);
			removeUserFromChannel(this, channel_name, mode_params);
			channel.addNewUser(mode_params);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, mode_params);
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		break;
	case 'k':
		if (mode_params.empty()) {
			std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
			addRplAndPollout(user, reply);
			return 0;
		}
		if (mode_action == '+') {
			if (!channel.getPassword().empty()) {
				std::string reply = ERR_KEYSET(user.getNickname(), channel.getChannelName());
				addRplAndPollout(user, reply);
				return 0;
			}
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, mode_params);
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
			channel.setPassword(mode_params);
		} else {
			if (mode_params != channel.getPassword())
				return 0; // do nothing
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, mode_params);
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
			std::string empty = "";
			channel.setPassword(empty);
		}
		break;
	case 'l':
		if (mode_action == '+') {
			if (mode_params.empty()) {
				std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
				addRplAndPollout(user, reply);
				return 0;
			}
			int limit = std::stoi(mode_params);
			if (limit < 0)
				return 0; // do nothing
			channel.setUsersLimit(limit);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, mode_params);
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		else {
			channel.setUsersLimit(-1);
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += MODE(channel_name, mode_action + mode_char, "");
			addRplAndPollout(user, reply);
			channel.broadcastMsg(this, user.getNickname(), reply);
		}
		break;
	default:
		break;
	}
	return 0;
}

static bool checkModeValid(std::string &mode) {
	if (mode.size() > 2)
		return false;
	if (mode.size() == 1 && !checkModeExist(mode[0]))
		return false;
	if (mode.size() == 2 && mode[0] != '+' && mode[0] != '-')
		return false;
	if (mode.size() == 2 && !checkModeExist(mode[1])) // check if second char is correct mode
		return false;
	return true;
}

static bool checkModeExist(char mode) {
	std::string supported_modes = "itkol";
	if (supported_modes.find(mode) == std::string::npos)
		return false;
	return true;
}