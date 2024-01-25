CC = c++

CFLAGS = -Wall -Wextra -Werror --std=98

NAME = ircserv

SOURCES = src/main.cpp src/server.cpp src/user.cpp src/channel.cpp  

all: $(NAME)

OBJECTS = $(SOURCES:.cpp=.o)

$(NAME): $(OBJECTS)
	$(CC) -o $(NAME) $(OBJECTS)

clean:
	rm -f $(OBJECTS)

fclean: clean
	rm -f $(NAME)

re: fclean all