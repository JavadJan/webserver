#ifndef Server_hpp
# define Server_hpp
#include <iostream>     // for std::cout
#include <string>       // for std::string
#include <vector>       // for std::vector
#include <cstring>      // for std::memcpy
#include <cerrno>       // for errno, std::strerror
#include <cstdlib>      // for std::exit

// POSIX headers (no C++ equivalents)
#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <poll.h>
#include <sys/types.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <map>
#include "../include/HttpRequest.hpp"

#define PORT 4242

class Config;
//class HttpRequest;

class Server{
	private:
		
		/* data for make connection, ip, port, sockets */
		int _port;
		std::vector<struct Config> servers;
		std::string host;

		struct sockaddr_in server_addr, client_addr;
		socklen_t client_len;
		char buffer[1024]; // this buffer should bo also per socket?
		int server_fd;
		int client_fd;

		/* track the state, smart state, persisit buffer per socket/client? */
		std::map<int, HttpRequest> http_req; // for every socket should be created an objet of request
		int start_body;

		/* for non-blocking */
		std::vector<pollfd> poll_fds;
		int poll_count; 

		/* private function */
		void accept_new_connection();
		void add_to_poll_fds(int new_fd);
		void del_from_poll_fds(int i);
		void read_data_from_socket(int i); // parsing heppen here
		int create_socket_bind();

		/* state machine function */
		void ParseFSM(int sock_fd); // parser the REQ.buffer->object
		void fsm(int sock_fd); // control strea string from TCP

		/* in each state I remove after complition STATE */
		void consume(size_t start, size_t end, int sock_fd);
		void parseRequestLine(std::string buf, int sock_fd);
		void parseHeader(std::string buf, int sock_fd);
		public:
			Server(std::vector<struct Config> serversConfig);
			void run();
			class ExceptionServer: public std::exception{
				public:
					const char *what() const throw();
			};
};


#endif