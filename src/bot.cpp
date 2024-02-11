#include "../includes/server.hpp"

void Server::chatbot(std::map<int, User>& users) {
	std::srand(std::time(0));

	if (users.empty()) {
		std::cout << "No users to send messages to." << std::endl;
		return;
	}

	int randomIndex = 0;
	if (users.size() > 1) {
		randomIndex = std::rand() % users.size();
	}
	std::map<int, User>::iterator it = users.begin();
	std::advance(it, randomIndex);
	User& selectedUser = it->second;

	std::vector<std::string> messages;
	messages.push_back("Hello! I'm waiting...\r\n");
	messages.push_back("Wake up!\r\n");
	messages.push_back("What's up?\r\n");
	messages.push_back("Are you sleeping?\r\n");
	messages.push_back("Please type something...\r\n");

	int randomMsgIndex = std::rand() % messages.size();
	std::string& selectedMsg = messages[randomMsgIndex];

	addRplAndPollout(selectedUser, selectedMsg);
}
