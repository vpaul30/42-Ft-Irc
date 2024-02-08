#include "../includes/channel.hpp"

Channel::Channel(std::string channel_name, User &user) : m_channel_name(channel_name) {
	m_users.push_back(user);
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