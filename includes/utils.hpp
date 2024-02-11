#include <iostream>
#include <string>
#include <sstream>
#include "server.hpp"
#include "user.hpp"
#include "replies.hpp"

class Server;
class User;

std::string intToString(int value);

// Checks if username is alphanumeric
bool validateUsername(std::string &username);

std::string registrationMessage(Server &server, User &user);

bool checkChannelExist(Server *server, std::string &channel_name);

bool checkUserInChannel(Server *server, std::string &channel_name, std::string &nickname);

bool checkUserChannelOperator(Server *server, std::string &channel_name, std::string &nickname);

bool checkUserExist(Server *server, std::string &nickname);

bool checkUserInvited(Server *server, std::string &channel_name, std::string &nickname);

void removeUserFromChannel(Server *server, std::string &channel_name, std::string &nickname);

void removeUserFromChannelUsers(Server *server, std::string &channel_name, std::string &nickname);

void removeUserFromChannelOperators(Server *server, std::string &channel_name, std::string &nickname);


std::pair<std::string, std::string> paramSplit(std::string params);

std::string formatTime(std::time_t raw);

std::string getTarget(std::string &params);
