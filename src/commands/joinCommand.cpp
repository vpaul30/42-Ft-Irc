#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

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
static bool checkChannelExist(Server *server, std::string &channel_name);

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
	
	for (int i = 0; i < channel_names.size(); i++) {
		if (isChannelNameValid(channel_names[i]) == false) {
			// ERR_NOSUCHCHANNEL (403)
			std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channel_names[i]);
			addRplAndPollout(user, reply);
		} else if (checkChannelExist(this, channel_names[i]) == true) {
			// channel exists, try to join
			std::cout << "trying to join " << channel_names[i] << std::endl;
		} else {
			// channel doesnt exist, create a new one
			std::cout << "creating " << channel_names[i] << std::endl;
			Channel new_channel(channel_names[i], user);
			m_channels.insert(std::pair<std::string, Channel>(channel_names[i], new_channel));
			std::string reply = prefix(user.getNickname(), user.getUsername(), user.getHostname());
			reply += JOIN(channel_names[i]);
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

static bool checkChannelExist(Server *server, std::string &channel_name) {
	std::map<std::string, Channel> &channels = server->getChannels();
	if (channels.find(channel_name) == channels.end())
		return false;
	return true;
}
