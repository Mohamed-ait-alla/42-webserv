CXX = c++
CXXFLAGS =  -Wall -Wextra -Werror -std=c++98
NAME = webserv

SRCS = 	./srcs/main.cpp \
		./srcs/serverCore/Logger.cpp \
		./srcs/serverCore/Helper.cpp \
		./srcs/serverCore/ConnectionManager.cpp \
		./srcs/serverCore/Epoll.cpp \
		./srcs/serverCore/Listener.cpp \
		./srcs/serverCore/Client.cpp \
		./srcs/serverCore/Server.cpp \
		./srcs/serverCore/CgiHandler.cpp \
		./srcs/serverCore/CgiUtils.cpp \
		./srcs/request/Request.cpp \
		./srcs/parse_config_file/parse_config_file.cpp \
		./srcs/response/Response.cpp \
		./srcs/response/initMimeTypes.cpp \
		./srcs/serverCore/webserv.cpp


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
