#include "../../includes/server.hpp"
#include "../../includes/replies.hpp"

/*

	Command: OPER
  	Parameters: <name> <password>

	Numeric Replies:
		ERR_NEEDMOREPARAMS (461)
		ERR_PASSWDMISMATCH (464)
		ERR_NOOPERHOST (491)
		RPL_YOUREOPER (381)

	Command Example:
  		OPER foo bar  -->  Attempt to register as an operator
    		               using a name of "foo" and the password "bar".
*/

static void extractOperParams(std::string &params, std::string &name, std::string &pass);
static bool checkOperIdAvailable(Server *server, int id);

int Server::operCommand(User &user, MsgInfo &msg_info) {
	if (user.getIsOperator() == true) // user is already an operator, simply return
		return 0;

	std::string name, pass;
	std::string reply;

	extractOperParams(msg_info.params, name, pass);
	if (name.empty() || pass.empty()) {
		// ERR_NEEDMOREPARAMS (461)
		reply = ERR_NEEDMOREPARAMS(user.getNickname(), msg_info.cmd);
		addRplAndPollout(user, reply);
		return 0;
	}
	// 
	for (int i = 0; i < m_oper_info.size(); i++) {
		if (name == m_oper_info[i].name) {
			if (checkOperIdAvailable(this, m_oper_info[i].id) == false) { // check if that name is not taken
				logMsg("Operator name already taken.", SERVER);
				return 0;
			}
			if (pass != m_oper_info[i].password) {
				// ERR_PASSWDMISMATCH (464)
				reply = ERR_PASSWDMISMATCH(user.getNickname());
				addRplAndPollout(user, reply);
				return 0;
			}
			if (user.getHostname() != m_oper_info[i].host) {
				// ERR_NOOPERHOST (491)
				reply = ERR_NOOPERHOST(user.getNickname());
				addRplAndPollout(user, reply);
				return 0;
			}
			// make user operator
			reply = RPL_YOUREOPER(user.getNickname());
			addRplAndPollout(user, reply);
			user.setIsOperator(true);
			user.setOperatorId(m_oper_info[i].id);

		}
	}

	return 0;
}

static void extractOperParams(std::string &params, std::string &name, std::string &pass) {
	if (params.empty())
		return;
	size_t pos;
	if ((pos = params.find(' ')) == std::string::npos)
		return;
	name = params.substr(0, pos);
	params.erase(0, pos);
	if ((pos = params.find_first_not_of(' ')) == std::string::npos)
		return;
	params.erase(0, pos); // erase all the space characters between <name> and <password>
	if ((pos = params.find(' ')) == std::string::npos) {
		pass = params;
	} else {
		pass = params.substr(0, pos);
	}
}

static bool checkOperIdAvailable(Server *server, int id) {
	// run through every user and check if his operatorId matches id, if so return false
	std::map<int, User> &users = server->getUsers();
	std::map<int, User>::iterator users_it = users.begin();
	for (; users_it != users.end(); users_it++) {
		if (users_it->second.getOperatorId() == id)
			return false;
	}
	return true;
}

