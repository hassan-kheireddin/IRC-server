NAME = ircserv

CXX = c++
CXXFLAGS = -Wall -Wextra -Werror -std=c++98
INCLUDES = -I./includes

SRCS = main.cpp \
	   srcs/Server.cpp \
	   srcs/Client.cpp \
	   srcs/Command.cpp \
	   srcs/Channel.cpp \
	   srcs/Message.cpp \

OBJS = $(SRCS:.cpp=.o)


all: $(NAME)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

$(NAME): $(OBJS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(NAME) $(OBJS)

clean:
	rm -f $(OBJS)

fclean: clean
	rm -f $(NAME)

re: fclean all

.PHONY: all clean fclean re