//#include "../include/ParsedData.hpp"
#include "../include/Server.hpp"
#include "../include/Config.hpp"

Server::Server(Config conf):_port(conf.getPort()),client_len(sizeof(client_addr)), server_fd(-1), client_fd(-1)
{
	// Prepare the address and port for the server socket
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;                     // IPv4
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
	server_addr.sin_port = htons(_port);

	/* for non-blocking */
	poll_count = 1;
	poll_size = 5;
}

int Server::create_socket_bind()
{
	int status;

	server_fd = socket(server_addr.sin_family, SOCK_STREAM, 0);
	if (server_fd == -1)
	{
		std::cout << "[Server] Socket Error: " << strerror(errno) << std::endl;
		return (-1);
	}
	// Bind socket to address and port
	status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr));
	if (status != 0)
	{
		std::cout << "[Server] Bind Error: " << strerror(errno) << std::endl;
		return (-1);
	}
	status = listen(server_fd, 10);
	if (status != 0)
	{
		fprintf(stderr, "[Server] Listen error: %s\n", strerror(errno));
		return (3);
	}
	//run();
	return 0;
}

void Server::run()
{
	create_socket_bind();
	int status;

	poll_size = 5;
	poll_fds = (struct pollfd *)calloc(poll_size + 1, sizeof *poll_fds);
	if (!poll_fds)
	{
		//return (4);
		throw std::runtime_error("[Server]: detect fd error");
	}
	// Add the listening server socket to array
	// with notification when the socket can be read
	poll_fds[0].fd = server_fd;
	poll_fds[0].events = POLLIN;
	poll_count = 1;

	while (1)
	{ // Main loop
		// Poll sockets to see if they are ready (2 second timeout)
		status = poll(poll_fds, poll_count, 2000);
		if (status == -1)
		{
			std::cout << "[Server] Poll error: \n" << strerror(errno);
			throw ExceptionServer();
		}
		else if (status == 0)
		{
			// None of the sockets are ready
			std::cout << "[Server] listening...\n";
			continue ;
		}
		// Loop on our array of sockets
		for (int i = 0; i < poll_count; i++)
		{
			if ((poll_fds[i].revents & POLLIN) != 1)
			{
				// The socket is not ready for reading
				// stop here and continue the loop
				continue ;
			}
			//printf("[%d] Ready for I/O operation\n", poll_fds[i].fd);
			// The socket is ready for reading!
			if (poll_fds[i].fd == server_fd)
			{
				// Socket is our listening server socket
				accept_new_connection();
			}
			else
			{
				// Socket is a client socket, read it
				read_data_from_socket(i);
			}
		}
	}
}

const char* Server::ExceptionServer::what() const throw(){
	return ("Faile to creation server!");
}

 // our server's port
void	Server::accept_new_connection()
{
	int		client_fd;
	char	msg_to_send[BUFSIZ];
	int		status;

	client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "[Server] Accept error: \n" << strerror(errno);
		return ;
	}
	add_to_poll_fds(client_fd); // no need to pass this fd
	printf("[Server] Accepted new connection on client socket %d.\n",
		client_fd);
	memset(&msg_to_send, '\0', sizeof msg_to_send);
	sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
	status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
	if (status == -1)
	{
		fprintf(stderr, "[Server] Send error to client %d: %s\n", client_fd,
			strerror(errno));
	}
}

void	Server::read_data_from_socket(int i)
{
	char	buffer[BUFSIZ];
	char	msg_to_send[BUFSIZ];
	int		bytes_read;
	int		status;
	int		dest_fd;
	int		sender_fd;

	sender_fd = poll_fds[i].fd;
	memset(&buffer, '\0', sizeof buffer);
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);
	if (bytes_read <= 0)
	{
		if (bytes_read == 0)
		{
			printf("[%d] Client socket closed connection.\n", sender_fd);
		}
		else
		{
			fprintf(stderr, "[Server] Recv error: %s\n", strerror(errno));
		}
		close(sender_fd); // Close socket
		del_from_poll_fds(i);
	}
	else
	{
		// Relays the received message to all connected sockets
		// but not to the server socket or the sender socket
		printf("[%d] Got message: %s", sender_fd, buffer);
		memset(&msg_to_send, '\0', sizeof msg_to_send);
		//sprintf(msg_to_send, "[%d] says: %s", sender_fd, buffer);
		snprintf(msg_to_send, sizeof(msg_to_send), "[%d] says: %s", sender_fd,
			buffer);
		for (int j = 0; j < poll_count; j++)
		{
			dest_fd = (poll_fds)[j].fd;
			if (dest_fd != server_fd && dest_fd != sender_fd)
			{
				status = send(dest_fd, msg_to_send, strlen(msg_to_send), 0);
				if (status == -1)
				{
					fprintf(stderr, "[Server] Send error to client fd %d: %s\n",
						dest_fd, strerror(errno));
				}
			}
		}
	}
}

// Add a new file descriptor to the pollfd array
void	Server::add_to_poll_fds(int new_fd)
{
	// If there is not enough room, reallocate the poll_fds array
	if (poll_count == poll_size)
	{
		poll_size *= 2; // Double its size
		poll_fds = (struct pollfd *)realloc(poll_fds, sizeof(*poll_fds)
				* (poll_size));
	}
	(poll_fds)[poll_count].fd = new_fd;
	(poll_fds)[poll_count].events = POLLIN;
	(poll_count)++;
}

// Remove an fd from the poll_fds array
void	Server::del_from_poll_fds(int i)
{
	// Copy the fd from the end of the array to this index
	(poll_fds)[i] = (poll_fds)[poll_count - 1];
	(poll_count)--;
}