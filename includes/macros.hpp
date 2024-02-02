#pragma once

#define prefix(nickname, username, hostname) (":" + nickname + "!" + username + "@" + hostname + " ")

// === ERRORS ===

#define ERR_NEEDMOREPARAMS(nickname, cmd) (":localhost 461" + nickname + " " + cmd + ": Not enough parameters.\r\n")
#define ERR_ALREADYREGISTERED(nickname) (":localhost 462 " + nickname + ": Already registered.\r\n")
#define ERR_PASSWDMISMATCH(nickname) (":localhost 464 " + nickname + ": Wrong password.\r\n")

#define ERR_NONICKNAMEGIVEN(nickname) (":localhost 431 " + nickname + ": No nickname given.\r\n")
#define ERR_ERRONEUSNICKNAME(nickname, new_nickname) (":localhost 432 " + nickname + " " + new_nickname + ": Erroneus nickname.\r\n")
#define ERR_NICKNAMEINUSE(nickname, new_nickname) (":localhost 433 " + nickname + " " + new_nickname + ": Nickname is already in use.\r\n")

// === REPLIES ===

#define NICK(old_nickname, username, hostname, new_nickname) ("changed nickname to " + new_nickname + "\r\n")