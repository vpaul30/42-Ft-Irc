#include "../includes/channel.hpp"

Channel::Channel(std::string channel_name, User &user) : m_channel_name(channel_name) {
	m_users.push_back(user);
}


std::string &Channel::getTopic() { return m_topic; }
void Channel::setTopic(std::string &topic) { m_topic = topic; }
std::vector<User> &Channel::getUsers() { return m_users; }
std::string &Channel::getChannelName() { return m_channel_name; }
void Channel::setChannelName(std::string &channel_name) { m_channel_name = channel_name; }