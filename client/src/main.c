#include "../include/client.h"

int	main(void)
{
	//	socket() → create socket
	// connect() → connect to server
	// send() / recv() → exchange data
	// close() → clean up
	int sockfd;
	struct sockaddr_in server_addr;
	char buffer[1024];
	const char *message = "Hello from client!";

	// 1. Create socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	// 2. Connect
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(8080);
	inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
	connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));

	// 4. Send message
    send(sockfd, message, strlen(message), 0);
    printf("[CLIENT] Sent: %s\n", message);

    // 5. Receive reply
    recv(sockfd, buffer, sizeof(buffer), 0);
    printf("[CLIENT] Received: %s\n", buffer);

    // 6. Close
    close(sockfd);

	// 4. Close
	close(sockfd);
}