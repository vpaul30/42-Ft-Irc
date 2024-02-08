#include "../includes/channel.hpp"

Channel::Channel() {
	m_channel_name = "";
	m_password = "";
	m_topic = "";
}

Channel::Channel(std::string channel_name, User &user) : m_channel_name(channel_name) {
	// m_users.push_back(user);
	m_operators.push_back(user);
}

std::string &Channel::getTopic() { return m_topic; }
void Channel::setTopic(std::string &topic, const std::string& nickname) {
	m_topic = topic;
	m_topicSetter = nickname;
	m_timeOfTopic = std::time(nullptr);
}
const std::string& Channel::getTopicSetter() { return m_topicSetter; }
std::time_t Channel::getTimeOfTopic() { return m_timeOfTopic; }

std::vector<User> &Channel::getUsers() { return m_users; }
std::vector<User> &Channel::getOperators() { return m_operators; }

std::string &Channel::getChannelName() { return m_channel_name; }
void Channel::setChannelName(std::string &channel_name) { m_channel_name = channel_name; }

std::string &Channel::getPassword() { return m_password; }
void Channel::setPassword(std::string &password) { m_password = password; }


void Channel::addNewUser(User &user) {
	m_users.push_back(user);
}

void Channel::broadcastMsg(Server *server, User &user_to_ignore, std::string msg) {
	std::cout << "BROADCAST MESSAGE <" << msg << "> TO " << m_operators.size() << " ops and " << m_users.size() << " users.\n";
	int i = 0;
	for (; i < m_operators.size(); i++) {
		if (m_operators[i].getNickname() == user_to_ignore.getNickname())
			continue;
		 server->addRplAndPollout(*(server->getUserByNickname(m_operators[i].getNickname())), msg);
		//  std::cout << "rpl buf(bc): " << m_operators[i].getRplBuffer() << std::endl;
	}
	i = 0;
	for (; i < m_users.size(); i++) {
		if (m_users[i].getNickname() == user_to_ignore.getNickname())
			continue;
		 server->addRplAndPollout(*(server->getUserByNickname(m_users[i].getNickname())), msg);
		//  std::cout << "rpl buf(bc): " << m_users[i].getRplBuffer() << std::endl;
	}
}
