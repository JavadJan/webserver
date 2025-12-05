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
	//poll_size = 5;
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
		std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
		return (3);
	}
	//run();
	return 0;
}

void Server::run()
{
	create_socket_bind();
	int status;

	poll_fds.clear();

    pollfd serverPoll;
    serverPoll.fd = server_fd;
    serverPoll.events = POLLIN;
    serverPoll.revents = 0;

    poll_fds.push_back(serverPoll);
	while (1)
	{ // Main loop
		// Poll sockets to see if they are ready (2 second timeout)
		for (std::vector<pollfd>::iterator it = poll_fds.begin(); it != poll_fds.end(); ++it)
        	it->revents = 0;
		status = poll(poll_fds.data(), poll_fds.size(), 2000);
		if (status == -1)
		{
			std::cerr << "[Server] Poll error: \n" << strerror(errno);
			throw ExceptionServer();
		}
		else if (status == 0)
		{
			// None of the sockets are ready
			std::cout << "[Server] listening...\n";
			continue ;
		}
		// Loop on our array of sockets
		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			if (!(poll_fds[i].revents & POLLIN))
				continue;
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
	//char	msg_to_send[BUFSIZ];
	std::string msg_to_send;
	int		status;

	client_fd = accept(server_fd, NULL, NULL);
	if (client_fd == -1)
	{
		std::cerr << "[Server] Accept error: \n" << strerror(errno);
		return ;
	}

	add_to_poll_fds(client_fd); // no need to pass this fd
	std::cout << "[Server] Accepted new connection on client socket" <<  client_fd << std::endl;
	//memset(&msg_to_send, '\0', sizeof msg_to_send);

	//sprintf(msg_to_send, "Welcome. You are client fd [%d]\n", client_fd);
	std::ostringstream oss;
	oss << "[" << client_fd << "] says: " << buffer;
	msg_to_send = oss.str();

	status = send(client_fd, msg_to_send.c_str(), msg_to_send.size(), 0);
	//status = send(client_fd, msg_to_send, strlen(msg_to_send), 0);
	if (status == -1)
	{
		std::cout << "[Server] Send error to client" << client_fd << strerror(errno) << std::endl;
	}
}

void	Server::read_data_from_socket(int i)
{
	char	buffer[BUFSIZ];
	char	msg_to_send[BUFSIZ];
	int		bytes_read;
	int		status = 0;
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
		std::cout << "[" << sender_fd << "] Got message: " << buffer;
		memset(&msg_to_send, '\0', sizeof msg_to_send);
		//sprintf(msg_to_send, "[%d] says: %s", sender_fd, buffer);
		//snprintf(msg_to_send, sizeof(msg_to_send), "[%d] says: %s", sender_fd,
		//	buffer);
		std::ostringstream oss;
		oss << "[" << sender_fd << "] says: " << buffer;
		std::string msg_to_send = oss.str();
		for (size_t j = 0; j < poll_fds.size(); j++)
		{
			dest_fd = (poll_fds)[j].fd;
			if (dest_fd != server_fd && dest_fd != sender_fd)
			{
				//status = send(dest_fd, msg_to_send., strlen(msg_to_send), 0);
				status = send(sender_fd, msg_to_send.c_str(), msg_to_send.size(), 0);
				if (status == -1)
				{
					fprintf(stderr, "[Server] Send error to client fd %d: %s\n",
						dest_fd, strerror(errno));
				}
			}
		}
	}
}

void Server::add_to_poll_fds(int new_fd)
{
    pollfd p;
    p.fd = new_fd;
    p.events = POLLIN;
    p.revents = 0;

    poll_fds.push_back(p);
}

void Server::del_from_poll_fds(int i)
{
    poll_fds[i] = poll_fds.back();
    poll_fds.pop_back();
}
