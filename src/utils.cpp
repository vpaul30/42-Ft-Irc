#include "../includes/utils.hpp"

std::string intToString(int value) {
	std::ostringstream oss;
	oss << value;
	return oss.str();
}

bool validateUsername(std::string &username) {
	for (size_t i = 0; i < username.size(); i++) {
		if (!std::isalnum(username[i]))
			return false;
	}
	return true;
}
