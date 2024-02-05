#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*
	Command: PRIVMSG
  	Parameters: <target> <text to be sent>

	Numeric Replies:
		ERR_NOSUCHNICK (401)
		ERR_NOSUCHSERVER (402)
		ERR_CANNOTSENDTOCHAN (404)
		ERR_TOOMANYTARGETS (407)
		ERR_NORECIPIENT (411)
		ERR_NOTEXTTOSEND (412)
		ERR_NOTOPLEVEL (413)
		ERR_WILDTOPLEVEL (414)
		RPL_AWAY (301)

	Command Example:
  		PRIVMSG Tony :Hey how are you? --> sends private message to Tony "Hey how are you?"
		PRIVMSG Tony Hey how are you? --> sends private message to Tony "Hey"
		PRIVMSG #myChannel :Hey guys --> sends message to everyone on channel #myChannel "Hey guys"

	PRIVMSG --> :localhost 411 <nickname> :No recipient given (PRIVMSG)
	PRIVMSG Tony --> :localhost 412 <nickname> :No text to send
	PRIVMSG <invalid nickname> text --> :localhost 401 <nickname> <invalid nickname> :No such nick/channel
*/

static bool checkTargetExist(std::string &params);

int Server::privmsgCommand(User &user, MsgInfo &msg_info) {
	// msg_info.params;
	return 0;
}

static bool checkTargetExist() {

}
