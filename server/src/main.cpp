#include "../include/server.hpp"
#include "../include/ParsedData.hpp"

enum STATE FSM(char buffer[], temp_data *data)
{
	(void)buffer;
	(void)data;
	std::string bufr = buffer;

	STATE state = REQ_LINE;
	while (state != DONE)
	{
		switch (state)
		{
		case REQ_LINE:
		{
			// fill method, version, path
			std::cout << "parsing the req_line\n";
			if (bufr.find("GET") == std::string::npos)
				state = ERROR;
			//state = HEADER;
			break ;
		}
		case HEADER:
		{
			std::cout << "parsing the header\n";
			state = BODY;
			// if exist key value it is header
			break ;
		}
		case BODY:
		{
			std::cout << "parsing the body\n";
			//state = DONE;
			break;
		}
		case DONE:
		{
			std::cout << "parsing the DONE\n";
			state = ERROR;
			break;
		}
		case ERROR:
		{
			std::cout << "ERROR\n";
			return ERROR;
			break;
		}
		default:
			break ;
		}
	}
	return DONE;
}

int	main(void)
{
	struct sockaddr_in server_addr, client_addr;
	socklen_t client_len = sizeof(client_addr);
	char buffer[1024];
	int sockfd;
	int client_fd;

	//--------------------------#
	//   		create socket   #
	//--------------------------#
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (!sockfd)
		printf("failed to creation socket\n");

	//--------------------------#
	//   		fill IP info    #
	//--------------------------#
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080); // port to binary,
										// in network use as binary format
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(server_addr.sin_zero, 0, sizeof(server_addr.sin_zero));

	//--------------------------#
	//   	make connection     #
	//--------------------------#
	// connect(sockfd,(struct sockaddr_in *)(&server_addr),sizeof(server_addr)); for cleint
	bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// 3. Listen; Start listening, queue up to 5 pending connections
	listen(sockfd, 5);

	// 4. Accept
	client_fd = accept(sockfd, (struct sockaddr *)&client_addr,
			&client_len);

	// 5. Communicate
	recv(client_fd, buffer, sizeof(buffer), 0);

	// run FSM, and parse buffer with state machine
	temp_data data;
	if (FSM(buffer, &data) == ERROR)
		std::cout << "return bad request page error\n";

	send(client_fd, "Hello from server!", 18, 0);
	const char *response = "Hello from server!";
	printf("[SERVER] Sent: %s\n", response);

	// 6. Close
	close(client_fd);
	close(sockfd);
}