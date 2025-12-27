CXX = c++
CXXFLAGS =  -Wall -Wextra -Werror -std=c++98 
NAME = webserv

# SRCS = ./ServerCore/ft_server.cpp
SRCS = main.cpp webserv.cpp ./server/server.cpp ./request/request.cpp ./response/response.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(NAME)

$(NAME): $(OBJS)
		$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

clean: 
	rm -rf $(OBJS)

fclean: clean
		rm -rf $(NAME)

re: fclean all

.SECONDARY: $(OBJS)
