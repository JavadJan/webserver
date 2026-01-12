//#include "../include/ParsedData.hpp"
#include "../include/Server.hpp"
#include "../include/Config.hpp"
#include "../include/HttpRequest.hpp"
#include "../include/ResponseHandler.hpp"

//--------------------------------------#
//		create server object			#
//--------------------------------------#

Server::Server(std::vector<struct Config> serversConfig)
:
//_port(serversConfig[0].port),
servers(serversConfig),
server_fd(-1)
{

	// fill here with config info? 
	// Prepare the address and port for the server socket
	
	//server_addr.sin_family = AF_INET;                     // IPv4
	//server_addr.sin_port = htons(_port);
	//server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // 127.0.0.1, localhost
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_family = AF_UNSPEC;        // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM;    // TCP
    hints.ai_flags = AI_PASSIVE;        // Automatically fills IP address

	for (size_t i = 0; i < servers.size(); i++)
	{
		/* code */
		struct addrinfo *tmp;
		std::cout << "port: " << servers[i].port << std::endl;
		int status = getaddrinfo(NULL, servers[i].port.c_str(), &hints, &tmp);
		if (status != 0) {
			std::cout << "getaddrinfo: " <<  gai_strerror(status) << std::endl;
			throw ExceptionServer();
		}
		res.push_back(tmp);
	}
	
}

static void set_nonblocking(int fd)
{
	// here the ftcntl() will return O_RDONLY, _WRONLY or O_NONBLOCK
	// F_GETFL: Get File Status Flags
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        throw std::runtime_error("fcntl F_GETFL failed");

	// if allready set as NONBLOCK then the flag will not set again
	// keep every things exist and add O_NONBLOCK; append mode
	// stand fot File CoNTroL
	//F_SETFL: Set File Status Flags
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
        throw std::runtime_error("fcntl F_SETFL failed");
}

//--------------------------------------#
//		define socket, bind, listen		#
//--------------------------------------#
/* define socket, bind to ip:port, listen to port */
int Server::create_socket_bind()
{
	int status;

	poll_fds.clear();
	for (size_t i = 0; i < servers.size(); i++)
	{
		/* code */
		server_fd = socket(res[i]->ai_family, res[i]->ai_socktype, res[i]->ai_protocol);
		if (server_fd < 0)
		{
			std::cout << "[Server] Socket Error: " << strerror(errno) << std::endl;
			continue;
			//return (-1);
		}
		/*Bind Error: Address already in use occurs because when a socket is closed,
			it stays in a kernel state called TIME_WAIT for several minutes. 
			setsockopt() tell the kernel reuse the port
		*/
		int opt = 1;
        if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
            std::cerr << "[Server] setsockopt Error: " << strerror(errno) << std::endl;
            close(server_fd);
            continue;
        }
		set_nonblocking(server_fd);
		// Bind socket to address and port
		status = bind(server_fd,  res[i]->ai_addr, res[i]->ai_addrlen);
		if (status != 0)
		{
			std::cout << "[Server] Bind Error: " << strerror(errno) << std::endl;
			return (-1);
		}
		//if (status == )
		status = listen(server_fd, 10);
		if (status != 0)
		{
			std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
			return (3);
		}
		
		// list of socket
		pollfd serverPoll;
		serverPoll.fd = server_fd; // add server socket to pollfd
		serverPoll.events = POLLIN; // it won't block recv();
		serverPoll.revents = 0;
	
		poll_fds.push_back(serverPoll);
		serverfd_config[server_fd] = servers[i]; // for every socket I created a server
	}
	if (poll_fds.empty())
        return -1;
	return 0;
}

void Server::run()
{
	create_socket_bind();
	int status;

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
			std::cout << "[Server on port: "
				 << servers[0].port << ", "
				 << servers[1].port << ", "
				 << servers[2].port
				 << "] listening ... " << std::endl;
			continue ; // skip the rest of loop after this line, start again to socket
		}
		// Loop on our array of sockets
		for (size_t i = 0; i < poll_fds.size(); i++)
		{
			// if there was not client socket
			if ((poll_fds[i].revents & POLLIN)) // POLLIN, readiness
			{
				// The socket is ready for reading!
				//if (poll_fds[i].fd == server_fd)
				if (serverfd_config.count(poll_fds[i].fd))
				{
					//a new client is trying to connect
					// Socket is our listening server socket
					accept_new_connection(poll_fds[i].fd);
				}
				else
				{
					// Socket is a client socket, read it
					read_data_from_socket(i);
				}
			}
			else if (poll_fds[i].revents & POLLOUT)
			{
				write_data_to_socket(i);
			}
		}
	}
}

const char* Server::ExceptionServer::what() const throw(){
	return ("Faile to creation server!");
}

 // our server's port
void	Server::accept_new_connection(int listen_fd)
{
	// in a loop accept server fd? 
	while (true)
	{
		int new_fd = accept(listen_fd, NULL, NULL);
		if (new_fd == -1) // under rule check the return value and then use errno
		{
			
			if (errno == EAGAIN || errno == EWOULDBLOCK)
				return; // normal
			std::cerr << "[Server] Accept error: \n" << strerror(errno);
			return;
		}
	
		set_nonblocking(new_fd);
	
		add_to_poll_fds(new_fd); // no need to pass this fd
		std::cout << "[Server] Accepted new connection on client socket" <<  new_fd << std::endl;
		http_req.insert(std::make_pair(new_fd, HttpRequest()));
		http_req[new_fd].setServerConfig(&serverfd_config[listen_fd]);
		
		//this->http_req[new_fd].clearBuffer();
	}
	
}

void	Server::read_data_from_socket(int i)
{
	char	chunk[BUFSIZ];
	//char	msg_to_send[BUFSIZ];
	int		bytes_read;
	//int		status = 0;
	//int		dest_fd;
	int		sender_fd;

	sender_fd = poll_fds[i].fd;

	memset(&chunk, '\0', sizeof(chunk));
	//recv() just reads whatever bytes are currently available in the kernel buffer for that socket.
	bytes_read = recv(sender_fd, chunk, BUFSIZ, 0);
		
	if (bytes_read > 0)
	{
		// Relays the received message to all connected sockets
		// but not to the server socket or the sender socket
		std::cout << "\n[" << sender_fd << "] Got message: start ----->\n" << chunk << "\n<----------end request\n\n";
		http_req[sender_fd].appendBuffer(chunk, bytes_read);		
	
		fsm(sender_fd); // in fsm get req with http_req[sender_fd]
		http_req[sender_fd].setClientSocket(sender_fd); // needs this socket to send response
		http_req[sender_fd].setPortServer(_port); //?????// there current server with[PORT] responses

		//std::cout << "state: " << clientState[sender_fd] << std::endl;
		if (http_req[sender_fd].getState() == HttpRequest::DONE 
			|| http_req[sender_fd].getState() == HttpRequest::ERROR)
		{
			// when recv() finished enabple POLLOUT to send()
			set_poll_events(sender_fd, POLLOUT); 

			std::cout << "\n\nHTTP REQ AFTER FSM: |" << http_req[sender_fd] << std::endl;
			/* create an object from response handler */
			ResponseHandler res;
			//res.controller(http_req[sender_fd], servers); // (req , res)=>{...}
			res.controller(http_req[sender_fd], *http_req[sender_fd].getServerConfig()); // (req , res)=>{...}
			res.finalize(http_req[sender_fd], *http_req[sender_fd].getServerConfig());
			//res.getResponse().setStatus(200); // set status code
			std::string response = res.getResponse().toString(); // make foramt http res to string
			std::cout << "response: " << response << std::endl;
			// fill the http_req for the clint == sender_fd, in write_data_to_fd() will send
			http_req[sender_fd].sendBuffer = response ; 
			http_req[sender_fd].sendOffset = 0;
			http_req[sender_fd].setState(HttpRequest::SENDING);
		}
		//else if (http_req[sender_fd].getState() == HttpRequest::ERROR)
		//{
		//	std::cout << "sending an error page\n";
		//	set_poll_events(sender_fd, POLLOUT); 
		//	ResponseHandler resError;

		//	//req, servers []
		//	//res.controller(http_req[sender_fd], servers); // (req , res)=>{...}
		//	resError.ErrorPage(http_req[sender_fd], *http_req[sender_fd].getServerConfig()); // (req , res)=>{...}
		//	//resError.getResponse().setStatus(http_req[sender_fd].getStatusCode()); // set status code for error
		//	//std::cout << "status code: " << "|" <<  http_req[sender_fd].getStatusCode() << std::endl;
		//	//std::cout << "status code: " << "|" <<  resError.getResponse().getStatus() << std::endl;
			
		//	//resError.getResponse().setBody(""); // set status code for error
		//	std::string response = resError.getResponse().toString(); // make foramt http res to string
		//	std::cout << "response: " << "|" << response  << std::endl;
		//	// fill the http_req for the clint == sender_fd, in write_data_to_fd() will send
		//	http_req[sender_fd].sendBuffer = response ; 
		//	http_req[sender_fd].sendOffset = 0;
		//	http_req[sender_fd].setState(HttpRequest::SENDING);
		//}
	}
	else
	{
		// if resive return -1 and set errno as EAGAIN, there is nothing to read
		//if (errno == EAGAIN || errno == EWOULDBLOCK)
       	//	return; // try later
		// && http_req[sender_fd].getBuffer().empty()
		if (bytes_read == 0)
		{
			close(sender_fd); // Close socket
			http_req.erase(sender_fd);
			std::cout << "[" << sender_fd << "] Client socket closed connection.\n";
		}
		else if (errno == EAGAIN || errno == EWOULDBLOCK)
			std::cout << "[Server] Recv: "<< strerror(errno);
		
		del_from_poll_fds(i); // 
	}
}

void Server::write_data_to_socket(int i)
{

	int fd = poll_fds[i].fd;
    HttpRequest &req = http_req[fd];

    // Try to send the remaining part of the buffer
    ssize_t n = send(fd, req.sendBuffer.data() + req.sendOffset, 
                     req.sendBuffer.size() - req.sendOffset, 0);

    if (n > 0) 
	{
        req.sendOffset += n; // update offset
    }
	else if (n == -1)
	{
		// not readiness
        if (errno == EAGAIN || errno == EWOULDBLOCK)
			return;
        // Handle actual error (close connection)
    	return;
    }

    // Is the whole buffer sent?
    if (req.sendOffset >= req.sendBuffer.size()) 
	{
        // FINISHED SENDING and clear the buffer
        req.sendBuffer.clear();
        req.sendOffset = 0;

        // Disable POLLOUT so we don't keep getting triggered
        set_poll_events(fd, POLLIN); 

        if (req.getHeader()["Connection"] == "close")
		{
            // Cleanup and close
			close(fd);                 // TCP layer
			del_from_poll_fds(i);             // event layer
			http_req.erase(fd);        // HTTP state cleanup
			req.setState(HttpRequest::REQ_LINE);
			return;
        }
		else
		{
            req.resetForNextRequest();
            req.setState(HttpRequest::REQ_LINE);
        }
    }
}


void Server::add_to_poll_fds(int cleint_fd)
{
    pollfd p;
    p.fd = cleint_fd;
    p.events = POLLIN;
    p.revents = 0;

    poll_fds.push_back(p);
}

void Server::del_from_poll_fds(int i)
{
    poll_fds[i] = poll_fds.back();
    poll_fds.pop_back();
}

void Server::set_poll_events(int fd, short events)
{
    for (size_t i = 0; i < poll_fds.size(); i++)
    {
        if (poll_fds[i].fd == fd)
        {
            poll_fds[i].events = events;
            return;
        }
    }
}

