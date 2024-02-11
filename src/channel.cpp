#include "../includes/channel.hpp"

Channel::Channel() {
	m_channel_name = "";
	m_topic = "";
	m_password = "";
	m_invite_only = false;
	m_users_limit = -1;
	m_topic_restriction = true;

}

Channel::Channel(std::string &channel_name, std::string &nickname) : m_channel_name(channel_name) {
	m_operators.push_back(nickname);
	m_topic = "";
	m_password = "";
	m_invite_only = false;
	m_users_limit = -1;
	m_topic_restriction = true;
}

std::string &Channel::getTopic() { return m_topic; }
void Channel::setTopic(std::string &topic, const std::string& nickname) {
	m_topic = topic;
	m_topicSetter = nickname;
	m_timeOfTopic = std::time(nullptr);
}
const std::string& Channel::getTopicSetter() { return m_topicSetter; }
std::time_t Channel::getTimeOfTopic() { return m_timeOfTopic; }

std::vector<std::string> &Channel::getUsers() { return m_users; }
std::vector<std::string> &Channel::getOperators() { return m_operators; }
std::vector<std::string> &Channel::getInvitedUsers() { return m_invited_users; }

std::string &Channel::getChannelName() { return m_channel_name; }
void Channel::setChannelName(std::string &channel_name) { m_channel_name = channel_name; }

std::string &Channel::getPassword() { return m_password; }
void Channel::setPassword(std::string &password) { m_password = password; }

bool Channel::getInviteOnly() { return m_invite_only; }
void Channel::setInviteOnly(bool value) { m_invite_only = value; }

int Channel::getUsersLimit() { return m_users_limit; }
void Channel::setUsersLimit(int limit) { m_users_limit = limit; }

bool Channel::getTopicRestriction() { return m_topic_restriction; }
void Channel::setTopicRestriction(bool value) { m_topic_restriction = value; }

void Channel::addNewUser(std::string &nickname) { m_users.push_back(nickname); }
void Channel::addNewOperator(std::string &nickname) { m_operators.push_back(nickname); }
void Channel::addInvitedUser(std::string &nickname) { m_invited_users.push_back(nickname); }


// Broadcasts message to everyone except user_to_ignore
void Channel::broadcastMsg(Server *server, std::string &nick_to_ignore, std::string &msg) {
	int i;
	for (i = 0; i < m_operators.size(); i++) {
		User *user = server->getUserByNickname(m_operators[i]);
		if (m_operators[i] == nick_to_ignore || user == NULL)
			continue;
		 server->addRplAndPollout(*user, msg);
	}

	for (i = 0; i < m_users.size(); i++) {
		User *user = server->getUserByNickname(m_users[i]);
		if (m_users[i] == nick_to_ignore || user == NULL)
			continue;
		 server->addRplAndPollout(*user, msg);
	}
}
