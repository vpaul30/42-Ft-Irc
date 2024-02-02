#pragma once

#define prefix(nickname, username) (":" + nickname + "!" + username + "@localhost")

// === ERRORS ===

#define ERR_NEEDMOREPARAMS(user, cmd) (":localhost 461" + user + " " + cmd + ": Not enough parameters.\r\n")
#define ERR_ALREADYREGISTERED(user) (":localhost 462 " + user + ": Already registered.\r\n")
#define ERR_PASSWDMISMATCH(user) (":localhost 464 " + user + ": Wrong password.\r\n")