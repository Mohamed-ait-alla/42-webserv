CXX = c++
CXXFLAGS =  -Wall -Wextra -Werror -g3 -std=c++98 
NAME = webserv

SRCS = 	main.cpp \
		./serverCore/Logger.cpp \
		./serverCore/Helper.cpp \
		./serverCore/ConnectionManager.cpp \
		./serverCore/Epoll.cpp \
		./serverCore/Listener.cpp \
		./serverCore/Client.cpp \
		./serverCore/Server.cpp \
		./serverCore/CgiHandler.cpp \
		./serverCore/CgiUtils.cpp \
		./request/Request.cpp \
		./parse_config_file/parse_config_file.cpp \
		./response/Response.cpp \
		./response/initMimeTypes.cpp \
		./webserv.cpp


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
