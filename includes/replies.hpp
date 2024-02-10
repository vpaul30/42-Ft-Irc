#pragma once

#define prefix(nickname, username, hostname) (":" + nickname + "!" + username + "@" + hostname + " ")

// === ERRORS ===

#define ERR_REGISTRATION ("ERROR :Closing Link: localhost (Registration failed)\r\n")

#define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461 " + nickname + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname) (":localhost 462 " + nickname + " :Already registered\r\n")
#define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + " :Wrong password\r\n")

#define ERR_NONICKNAMEGIVEN(nickname) (":localhost 431 " + nickname + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, new_nickname) (":localhost 432 " + nickname + " " + new_nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nickname, new_nickname) (":localhost 433 " + nickname + " " + new_nickname + " :Nickname is already in use\r\n")

#define ERR_NORECIPIENT(nickname) (":localhost 411 " + nickname + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTTOSEND(nickname) (":localhost 412 " + nickname + " :No text to send\r\n")
#define ERR_NOSUCHNICK(nickname, target_nickname) (":localhost 401 " + nickname +  " " + target_nickname + " :No such nick/channel\r\n")

#define ERR_NOSUCHCHANNEL(nickname, channel) (":localhost 403 " + nickname + " " + channel + " :No such channel\r\n")
#define ERR_CANNOTSENDTOCHAN(nickname, channel) (":localhost 404 " + nickname + " " + channel + " :Cannot send to nick/channel\r\n")
#define ERR_NOTONCHANNEL(nickname, channel) (":localhost 442 " + nickname + " " + channel + " :You're not on that channel\r\n")
#define ERR_CHANOPRIVSNEEDED(nickname, channel) (":localhost 482 " + nickname + " " + channel + " :You're not channel operator\r\n")
#define ERR_USERNOTINCHANNEL(nickname, who, channel) (":localhost 441 " + nickname + " " + who + " " + channel + " :They aren't on that channel\r\n")
#define ERR_USERONCHANNEL(nickname, who, channel) (":localhost 443 " + nickname + " " + who + " " + channel + " :is already on channel\r\n")

#define ERR_NOOPERHOST(nickname) (":localhost 491 " + nickname + " :No operator for your host\r\n")

#define ERR_UNKNOWNCOMMAND(nickname, cmd) (":localhost 421 " + nickname + " " + cmd + " :Unknown command\r\n")

// === REPLIES ===

#define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the 42 Internet Relay Chat Network " + nickname + ".\r\n")
#define RPL_YOURHOST(nickname) (":localhost 002 " + nickname + " :Your host is FT_IRC, running version 1.0\r\n")
#define RPL_CREATED(nickname, datetime) (":localhost 003 " + nickname + " :This server was created " + datetime + "\r\n")
#define RPL_MYINFO(nickname) (":localhost 004 " + nickname + " FT_IRC 1.0 oiws itkol k\r\n")
#define RPL_ISUPPORT(nickname) (":localhost 005 " + nickname + " MAXTARGETS=1 NICKLEN=12 TARGMAX  :are supported by this server\r\n")

#define RPL_NOTOPIC(nickname, channel) (":localhost 331 " + nickname + " " + channel + " :No topic is set\r\n")
#define RPL_TOPIC(nickname, channel, topic) (":localhost 332 " + nickname + " " + channel + " " + topic + "\r\n")
#define RPL_TOPICWHOTIME(nickname, channel, who, time) (":localhost 333 " + nickname + " " + channel + " " + who + " " + time + "\r\n")
#define RPL_INVITING(nickname, who, channel) (":localhost 341 " + nickname + " " + who + " " + channel + "\r\n")
#define RPL_YOUREOPER(nickname) (":localhost 381 " + nickname + " :You are now an operator of FT_IRC\r\n")

#define NICK(old_nickname, username, hostname, new_nickname) ("changed nickname to " + new_nickname + "\r\n") // should be sent with prefix
#define PRIVMSG(target, message) ("PRIVMSG " + target + " :" + message + "\r\n") // should be sent with prefix
#define NOTICE(target, message) ("NOTICE " + target + " :" + message + "\r\n") // should be sent with prefix
#define JOIN(channel) ("JOIN " + channel + "\r\n") // should be sent with prefix
#define QUIT(nickname) ("QUIT : " + nickname + " closed the connection\r\n") // should be sent with prefix
#define TOPIC(channel, topic) ("TOPIC " + channel + " " + topic + "\r\n") // should be sent with prefix
#define PART(channel) ("PART " + channel + "\r\n") // should be sent with prefix
#define INVITE(nickname, channel) ("INVITE " + nickname + " :" + channel + "\r\n") // should be sent with prefix
#define KICK(channel, kick_nickname, comment) ("KICK " + channel + " " + kick_nickname + " :" + comment + "\r\n") // should be sent with prefix