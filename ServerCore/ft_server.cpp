#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 8080

int main()
{
	// var declarations
	int 				serverSocket, newSocket;
	ssize_t				dataReaded;
	struct sockaddr_in	address;
	socklen_t			addrlen = sizeof(address);

	// init response header
	const char*			hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello, World!";

	// try catch block
	try
	{
		// step 1: create socket fd
		if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0)
		{
			throw std::runtime_error("Error occurs in socket()");
		}
	
		// step 2: Identifying socket
		address.sin_family = AF_INET;
		address.sin_addr.s_addr = INADDR_ANY;
		address.sin_port = htons(PORT);
	
		std::memset(address.sin_zero, '\0', sizeof(address.sin_zero));
	
		if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0)
		{
			close (serverSocket);
			throw std::runtime_error("Error occurs in bind()");
		}
	
		// step 3: Waiting for connections
		if (listen(serverSocket, 10) < 0)
		{
			throw std::runtime_error("Error occurs in listen()");
		}
		while (1)
		{
			std::cout << "\n+++++++ Waiting for new connection +++++++" << std::endl;
			if ((newSocket = accept(serverSocket, (struct sockaddr *)&address, &addrlen)) < 0)
			{
				throw std::runtime_error("Error occurs in accept()");
			}
	
			// get the ip and port the connected client
			char*	clientIP= inet_ntoa(address.sin_addr);
			int		clientPort = ntohs(address.sin_port);
			std::cout << "\n connection received from client " << clientIP << ":" << clientPort << std::endl;
	
			// step 4: Send and receive data
			char	buffer[30000];
			std::memset(buffer, 0, sizeof(buffer));
			dataReaded = read(newSocket, buffer, sizeof(buffer) - 1);
			if (dataReaded > 0)
			{
				buffer[dataReaded] = '\0';
				std::cout << buffer << std::endl;

				write(newSocket, hello, std::strlen(hello));
				std::cout << "---------------------- Hello msg sent ----------------------" << std::endl;
			}
			else
			{
				std::cout << " Error or no data received from client" << std::endl;
			}
			
			// step 5: close connection
			close(newSocket);
		}
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return (0);

}