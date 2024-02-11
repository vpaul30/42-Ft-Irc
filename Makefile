CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98
# FLAGS = -std=c++98

NAME = ircserv

SOURCES = src/main.cpp src/server.cpp src/user.cpp src/channel.cpp src/utils.cpp \
		src/commands/passCommand.cpp src/commands/nickCommand.cpp src/commands/userCommand.cpp \
		src/commands/privmsgCommand.cpp src/commands/joinCommand.cpp src/commands/topicCommand.cpp \
		src/commands/partCommand.cpp src/commands/noticeCommand.cpp src/commands/operCommand.cpp \
		src/commands/inviteCommand.cpp src/commands/kickCommand.cpp src/commands/modeCommand.cpp \
		src/bot.cpp

OBJECTS = $(SOURCES:.cpp=.o)

$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS)

all: $(NAME)

.cpp.o:
	$(CC) $(FLAGS) -c $< -o $(<:.cpp=.o)

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all