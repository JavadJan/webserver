#include "../include/Server.hpp"
#include "../include/Config.hpp"
//#include "../include/ParsedData.hpp"
#include <fcntl.h>


/* int main (int argc, char **argv)
{
	std::string config_Path = "config.conf";
	if (argc == 2)
		config_Path = argv[1];
	
		Config Config(config_Path);
		std::cout << "Host: " << Config.getHost() << std::endl;
		std::cout << "Port: " << Config.getPort() << std::endl;
	return 0;
} */



#include "Config.hpp"
#include "Server.hpp"

int main()
{
    Config conf("server.conf");
    Server srv(conf);

    try {
        srv.run();
    } catch (const Server::ExceptionServer &e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}












/* 

int	main(void)
{
	//struct sockaddr_in server_addr, client_addr;
	//socklen_t client_len = sizeof(client_addr);
	//char buffer[1024];
	//int sockfd;
	//int client_fd;

	////--------------------------#
	////   		create socket   #
	////--------------------------#
	//sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//if (!sockfd)
	//	printf("failed to creation socket\n");
	//// convert socket to non-blocking, bloked: the program exe line-by-line then wait for client, if not client it wait forever
	//fcntl(sockfd, F_SETFL, O_NONBLOCK);

	////--------------------------#
	////   		fill IP info    #
	////--------------------------#
	//server_addr.sin_family = AF_INET;
	//server_addr.sin_port = htons(8080); // port to binary,
	//									// in network use as binary format
	//server_addr.sin_addr.s_addr = INADDR_ANY;
	//memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	////--------------------------#
	////   	make connection     #
	////--------------------------#
	//// connect(sockfd,(struct sockaddr_in *)(&server_addr),sizeof(server_addr)); for cleint
	//bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	//// 3. Listen; Start listening, queue up to 5 pending connections
	//listen(sockfd, 5);

	//// 4. Accept
	//client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
	//		&client_len);

	//// 5. Communicate
	//int res = recv(client_fd, buffer, sizeof(buffer), 0);
	//if (res == -1)
    //    std::cerr << "recv failed: " << strerror(errno) << std::endl;
	//else if (res == 0)
	//	std::cout << "remote connection has closed!\n" << std::endl;

	//// run FSM, and parse buffer with state machine
	//temp_data data;
	//if (FSM(buffer, &data) == ERROR)
	//	std::cout << "return bad request page error\n";

	//send(client_fd, "Hello from server!", 18, 0);

	//const char *response = "Hello from server!";
	//printf("[SERVER] Sent: %s\n", response);

	//// 6. Close
	//close(client_fd);
	//close(sockfd);
	Config conf;
	Server server(conf);
	//server.create_socket_bind();
	server.run();

	return 0;
} */