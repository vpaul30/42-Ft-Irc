#pragma once

#define prefix(nickname, username, hostname) (":" + nickname + "!" + username + "@" + hostname + " ")

// === ERRORS ===

#define ERR_REGISTRATION ("ERROR :Closing Link: localhost (Registration failed)\r\n")

#define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461" + nickname + " " + cmd + " :Not enough parameters\r\n")
#define ERR_ALREADYREGISTERED(nickname) (":localhost 462 " + nickname + " :Already registered\r\n")
#define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + " :Wrong password\r\n")

#define ERR_NONICKNAMEGIVEN(nickname) (":localhost 431 " + nickname + " :No nickname given\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, new_nickname) (":localhost 432 " + nickname + " " + new_nickname + " :Erroneus nickname\r\n")
#define ERR_NICKNAMEINUSE(nickname, new_nickname) (":localhost 433 " + nickname + " " + new_nickname + " :Nickname is already in use\r\n")

#define ERR_NORECIPIENT(nickname) (":localhost 411 " + nickname + " :No recipient given (PRIVMSG)\r\n")
#define ERR_NOTEXTTOSEND(nickname) (":localhost 412 " + nickname + " :No text to send\r\n")
#define ERR_NOSUCHNICK(nickname, target_nickname) (":localhost 401 " + nickname +  " " + target_nickname + ":No such nick/channel\r\n")

// === REPLIES ===

#define RPL_WELCOME(nickname) (":localhost 001 " + nickname + " :Welcome to the 42 Internet Relay Chat Network " + nickname + ".\r\n")
#define RPL_YOURHOST(nickname) (":localhost 002 " + nickname + " :Your host is FT_IRC, running version 1.0\r\n")
#define RPL_CREATED(nickname, datetime) (":localhost 003 " + nickname + " :This server was created " + datetime + "\r\n")
#define RPL_MYINFO(nickname) (":localhost 004 " + nickname + " FT_IRC 1.0 oiws itkol k\r\n")
#define RPL_ISUPPORT(nickname) (":localhost 005 " + nickname + " MAXTARGETS=1 NICKLEN=12  :are supported by this server\r\n")

#define NICK(old_nickname, username, hostname, new_nickname) ("changed nickname to " + new_nickname + "\r\n")
#define PRIVMSG(target_nickname, message) ("PRIVMSG " + target_nickname + " :" + message + "\r\n");