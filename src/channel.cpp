#include "../includes/channel.hpp"

Channel::Channel(std::string channel_name, User &user) : m_channel_name(channel_name) {
	m_users.push_back(user);
	m_operators.push_back(user);
}

std::string &Channel::getTopic() { return m_topic; }
void Channel::setTopic(std::string &topic) { m_topic = topic; }

std::vector<User> &Channel::getUsers() { return m_users; }
std::vector<User> &Channel::getOperators() { return m_operators; }

std::string &Channel::getChannelName() { return m_channel_name; }
void Channel::setChannelName(std::string &channel_name) { m_channel_name = channel_name; }

std::string &Channel::getPassword() { return m_password; }
void Channel::setPassword(std::string &password) { m_password = password; }