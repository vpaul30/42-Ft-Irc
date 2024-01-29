CC = c++

FLAGS = -Wall -Wextra -Werror -std=c++98

NAME = ircserv

SOURCES = src/main.cpp src/server.cpp src/user.cpp src/channel.cpp src/utils.cpp

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