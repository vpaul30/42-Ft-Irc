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