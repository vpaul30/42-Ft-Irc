#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

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
        ERR_NOTONCHANNEL (442)
        ERR_NOSUCHCHANNEL (403)
        ERR_UNKNOWNMODE (472)
        ERR_KEYSET (467)

    Command Example:
        MODE #Finnish +im               ; Makes #Finnish channel moderated and 'invite-only'.

        MODE #Finnish +o Kilroy         ; Gives 'chanop' privileges to Kilroy on channel #Finnish.

        MODE #42 +k oulu                ; Set the channel key to "oulu".

        MODE #eu-opers +l 10            ; Set the limit for the number of users on channel to 10.
*/

static std::pair<std::string, std::string> paramSplit(const std::string &params);
static char modeNotExist(std::string mode);

int Server::modeCommand(User &user, MsgInfo &msg_info)
{
    // PARSE MODE ARGS, STOP IF NOT RIGHT
    std::pair<std::string, std::string> splitParams = paramSplit(msg_info.params);
    std::string channelName = splitParams.first;
    std::string modeAndModeParams = splitParams.second;

    if (msg_info.params.empty() || channelName.empty()) {
        // ERR_NEEDMOREPARAMS (461)
        std::string reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
        addRplAndPollout(user, reply);
        return 0;
    }

    std::pair<std::string, std::string> splitModeParams = paramSplit(msg_info.params);
	std::string mode = splitModeParams.first;
	std::string modeParams = splitModeParams.second;
	std::string nickname = nullptr;

	if (char unknownMode = modeNotExist(mode) != NULL) {
		std::string reply = ERR_UNKNOWNMODE(user.getNickname(), unknownMode);
		addRplAndPollout(user, reply);
		return 0;
	}

    // CHECK IF CHANNEL IS EXISTS AND HAVE USER AS AN OPERATOR
	if (channelNotExistent(channelName)) {
		std::string reply = ERR_NOSUCHCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (!userIsOperator(user, channelName)) {
		std::string reply = ERR_CHANOPRIVSNEEDED(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

	if (!nickInChannel(user, channelName)) {
		std::string reply = ERR_NOTONCHANNEL(user.getNickname(), channelName);
		addRplAndPollout(user, reply);
		return 0;
	}

    // CHANGE CHANNEL MODE
	bool exitLoop = false;
    Channel &channel = m_channels[channelName];
	
	for (int i = 0; i < mode.length() && exitLoop; i++) {
		switch (mode[i])
		{
		case 'i':
			
			break;

		case 't':

			break;
		
		case 'o':
			if (!nickname.empty() && nickNotExistent(nickname)) {
				std::string reply = ERR_NOSUCHNICK(user.getNickname(), nickname);
				addRplAndPollout(user, reply);
				return 0;
			}
			break;
		
		case 'k':
			if (!channel.getPassword().empty()) {
				std::string reply = ERR_KEYSET(user.getNickname(), channel.getChannelName());
				addRplAndPollout(user, reply);
				return 0;
			}
			else {
				channel.setPassword(modeParams);
			}
			break;
		
		case 'l':

			break;
		
		default:
			break;
		}
	}
}

static void processModes() {

}

static char modeNotExist(std::string mode) {
	std::string supportedModes = "iotkl";

	for (int i = 0; i < mode.size(); i++) {
		if (supportedModes.find(mode[i]) == std::string::npos) {
			return mode[i];
		}
	}
	return NULL;
}

static std::pair<std::string, std::string> paramSplit(const std::string &params)
{
    size_t firstSpace = params.find(' ');
    if (firstSpace != std::string::npos)
    {
        std::string firstPart = params.substr(0, firstSpace);
        std::string secondPart = params.substr(firstSpace + 1);
        return std::make_pair(firstPart, secondPart);
    }
    else
    {
        return std::make_pair(params, "");
    }
}
