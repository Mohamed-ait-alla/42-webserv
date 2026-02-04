CXX = c++
CXXFLAGS =  -Wall -Wextra -Werror -std=c++98
NAME = webserv

SRCS = 	./src/main.cpp \
		./src/webserv.cpp \
		./src/configParser/configParser.cpp \
		./src/server/Listener.cpp \
		./src/server/Epoll.cpp \
		./src/server/ConnectionManager.cpp \
		./src/server/Server.cpp \
		./src/Client.cpp \
		./src/cgi/CgiHandler.cpp \
		./src/cgi/CgiUtils.cpp \
		./src/http/Request.cpp \
		./src/http/Response.cpp \
		./src/http/initMimeTypes.cpp \
		./src/utils/Logger.cpp \
		./src/utils/Helper.cpp


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
