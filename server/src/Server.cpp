//#include "../include/ParsedData.hpp"
#include "../include/Server.hpp"
#include "../include/Config.hpp"
#include "../include/HttpRequest.hpp"

Server::Server(Config conf):_port(conf.getPort()),client_len(sizeof(client_addr)), server_fd(-1), client_fd(-1)
{
	// Prepare the address and port for the server socket
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;                     // IPv4
	server_addr.sin_port = htons(_port);
	server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost

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
    serverPoll.fd = server_fd; // add server socket to pollfd
    serverPoll.events = POLLIN; // it won't block recv();
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
			std::cout << "[Server on port: " << _port << "] listening ... " << std::endl;
			continue ; // skip the rest of loop after this line, start again to socket
		}
		// Loop on our array of sockets
		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			// if there was not client socket
			if (!(poll_fds[i].revents & POLLIN))
				continue;

			// The socket is ready for reading!
			if (poll_fds[i].fd == server_fd)
			{
				//a new client is trying to connect
				// Socket is our listening server socket
				accept_new_connection();
			}
			else
			{
				std::cout <<"client fd" << client_fd << "------------------" << i << std::endl;
				// Socket is a client socket, read it
				HttpRequest request;
				// in reading time create HttpRequest obj
				read_data_from_socket(i, request);
				
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

	clientState[client_fd] = REQ_LINE;
	recvBuffer[client_fd].clear();

}

void	Server::read_data_from_socket(int i, HttpRequest &request)
{
	char	buffer[BUFSIZ];
	char	msg_to_send[BUFSIZ];
	int		bytes_read;
	int		status = 0;
	int		dest_fd;
	int		sender_fd;

	


	sender_fd = poll_fds[i].fd;
	memset(&buffer, '\0', sizeof(buffer));
	bytes_read = recv(sender_fd, buffer, BUFSIZ, 0);

	if (clientState[sender_fd] == DONE || clientState[sender_fd] == ERROR) {
		clientState[sender_fd] = REQ_LINE;                   // fresh start for new request
	}
	

	if (bytes_read <= 0 && recvBuffer[sender_fd].empty())
	{
		if (bytes_read == 0)
			std::cout << "[" << sender_fd << "] Client socket closed connection.\n";
		else
			std::cout << "[Server] Recv error: "<< strerror(errno);
		
		close(sender_fd); // Close socket
		del_from_poll_fds(i);
	}
	else
	{
		// Relays the received message to all connected sockets
		// but not to the server socket or the sender socket
		std::cout << "[" << sender_fd << "] Got message: start ----->\n" << buffer << "\n<----------end request\n";
		recvBuffer[sender_fd].append(buffer, bytes_read); // append to cleint_fd
		//std::cout << "send to fsm: " << recvBuffer[sender_fd] << std::endl;
		
		fsm(recvBuffer[sender_fd], sender_fd);

		//std::cout << "state: " << clientState[sender_fd] << std::endl;

		if (clientState[sender_fd] == DONE)
		{
			request = ParseFSM(recvBuffer[sender_fd]); // attention here for fsm
			recvBuffer[sender_fd].clear();
			clientState[sender_fd] = REQ_LINE;
			std::cout << "\n\nHTTP REQ: " << request << std::endl;
		}


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
					std::cout << "[Server] Send error to client fd" << strerror(errno) << std::endl;
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

/* 
	std::vector<pollfd> poll_fds;
	int num = poll(poll_fds.data(), poll_fds.size(), 2000);
	num = 0		-> no detect fd to read; so the timeout has expired
	num = -1	-> error happend
	num > 0		-> fins fd to read

	struct pollfd {
		int   fd;        -> File descriptor to monitor
		short events;    -> Requested events (what you want to watch for)
		short revents;   -> Returned events (what actually happened)
	};
	fd : the file descriptor you want to watch for, e.g., file, pipe, socket
	events: a bitmask event you want to watch for. 
			e.g., 	POLLIN : ready for reading (it won't block recv() ) 0001
					POLLOUT: ready for writing (it won't block send() ) 0100
	revents:	fild by kernal after poll();
				tells you which event happend on that file descriptor

	bitmask? 
		A bitmask is just an integer where each bit represents a flag (a yes/no condition).
		Instead of storing multiple booleans separately, you pack them into one number.
		You can then use bitwise operators (|(OR), &(AND)) to combine or test flags.

	--------------------------
	we’re checking all sockets that are ready (poll_fds[i].revents & POLLIN).
	But not all sockets are the same:
		- Listening socket → readiness means a new client wants to connect. You must call accept().
		- Client socket → readiness means this client sent data. You must call recv()

*/

/* 
	STEP 1 — HTTP request parsing (FSM) first
		- method
		- path
		- protocol
		- header parsing
		- detect end of headers
		- detect body based on Content-Length
		TEST:
			• curl
			• browser
			• raw netcat
			• split TCP chunks
			• malformed requests	

	STEP 2 — Config parser

		- build a simple object tree	- 
		- fill server roots, ports, routes, etc.

	STEP 3 — Request handler
		- match URI to config
		- check method allowed
		- build static file path
		- decide between static file, directory, or CGI

	STEP 4 — Response generator
		- build status line
		- build headers
		- build body
		- store in per-client write buffer

	STEP 5 — Non-blocking POLLOUT sending
		- handle partial sends
		- flush buffer
		- close connection if needed

	STEP 6 — CGI execution
		- fork
		- execve
		- read pipes non-blocking
*/