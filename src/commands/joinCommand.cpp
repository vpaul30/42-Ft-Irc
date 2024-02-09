#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"
// #include "../../includes/utils.hpp"

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
// static bool checkChannelExist(Server *server, std::string &channel_name);
// static bool checkUserInChannel(Server *server, std::string &channel_name, std::string &nickname);

int Server::joinCommand(User &user, MsgInfo &msg_info) {
	if (msg_info.params.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), "JOIN");
		addRplAndPollout(user, reply);
		return 0;
	}

	std::vector<std::string> channel_names;
	getChannelNames(channel_names, msg_info.params);
	std::vector<std::string> channel_keys;
	getChannelKeys(channel_keys, msg_info.params);

	std::string channel_name;
	for (int i = 0; i < channel_names.size(); i++) {
		channel_name = channel_names[i];
		if (isChannelNameValid(channel_name) == false) {
			// ERR_NOSUCHCHANNEL (403)
			std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channel_name);
			addRplAndPollout(user, reply);
		} else if (checkChannelExist(this, channel_name) == true) { // channel exists, try to join
			if (checkUserInChannel(this, channel_name, user.getNickname()) == true) // if user is already in that channel, simply ignore join command
				return 0;
			Channel &channel_to_join = m_channels[channel_name];
			if (!channel_to_join.getPassword().empty()) {
				std::string &password = channel_to_join.getPassword();
				if (channel_keys.size() < i + 1) {
					std::cout << "No key specified to join the channel: " << channel_name << std::endl;
					continue;
				} else if (channel_keys[i] != password) {
					std::cout << "Wrong password. Cannot join the channel: " << channel_name << std::endl;
					continue;
				}
			} else if (channel_to_join.getInviteOnly() == true) {
				; // check if user is invited then join
			} else if (channel_to_join.getUsersLimit() != -1) {
				; // check if there is space for user then join
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
		} else { // channel doesn't exist, create a new one
			Channel new_channel(channel_name, user.getNickname());
			m_channels.insert(std::pair<std::string, Channel>(channel_name, new_channel));
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += JOIN(channel_name);
			addRplAndPollout(user, reply);
		}
	}

	return 0;
}

static void getChannelNames(std::vector<std::string> &channel_names, std::string &params) {
	std::string copy = params;
	std::string word;
	size_t pos;

	while (true) {
		pos = copy.find_first_of(" ,");
		if (pos == std::string::npos) { // only 1 word in copy --> push it and break
			if (!copy.empty())
				channel_names.push_back(copy);
			break;
		} else if (copy[pos] == ' ') { // ' ' means there are no more channel_names after it --> push it and break
			word = copy.substr(0, pos);
			if (!word.empty())
				channel_names.push_back(word);
			break;
		} else { // ','
			word = copy.substr(0, pos);
			if (!word.empty())
				channel_names.push_back(word); // ',' means there are more channel_names after it --> push it and erase
			copy.erase(0, pos + 1);
		}
	}
}

static void getChannelKeys(std::vector<std::string> &channel_keys, std::string &params) {
	std::string copy = params;
	std::string word;
	size_t pos;

	if ((pos = copy.find(' ')) == std::string::npos) // no ' ' in params means there are no keys
		return;
	copy.erase(0, pos + 1); // erase everything until ' ' included

	while(true) {
		pos = copy.find_first_of(" ,");
		if (pos == std::string::npos) { // only 1 word in copy --> push it and break
			if (!copy.empty())
				channel_keys.push_back(copy);
			break;
		} else if (copy[pos] == ' ') { // ' ' means there are no more channel_keys after it --> push it and break
			word = copy.substr(0, pos);
			if (!word.empty())
				channel_keys.push_back(word);
			break;
		} else { // ','
			word = copy.substr(0, pos);
			if (!word.empty())
				channel_keys.push_back(word); // ',' means there are more channel_keys after it --> push it and erase
			copy.erase(0, pos + 1);
		}
	}
}

static bool isChannelNameValid(std::string &channel_name) {
	if (channel_name[0] != '#')
		return false;
	if (channel_name.size() == 1)
		return false;
	return true;
}
