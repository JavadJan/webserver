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
		/* state for state machine */
		enum	STATE
		{
			REQ_LINE,
			HEADER,
			BODY,
			DONE,
			ERROR
		};

		/* data for make connection, ip, port, sockets */
		int _port;
		struct sockaddr_in server_addr, client_addr;
		socklen_t client_len;
		char buffer[1024]; // this buffer should bo also per socket?
		int server_fd;
		int client_fd;

		/* track the state, smart state, persisit buffer per socket/client? */
		std::map<int, STATE> clientState; // keep state per client
		std::map<int, std::string> recvBuffer;
		std::map<int, size_t> conten_len; // this field has defiend two times, another in http_req
		std::map<int, HttpRequest> http_req; // for every socket should be created an objet of request
		int start_body;

		/* for non-blocking */
		std::vector<pollfd> poll_fds;
		int poll_count; 

		/* private function */
		void accept_new_connection();
		void add_to_poll_fds(int new_fd);
		void del_from_poll_fds(int i);
		void read_data_from_socket(int i, HttpRequest &request); // parsing heppen here
		int create_socket_bind();

		/* state machine function */
		HttpRequest ParseFSM(std::string req); // parser the req
		void fsm(std::string recieve, int sock_fd); // control strea string from TCP

		public:
			Server(Config config);
			void run();
			class ExceptionServer: public std::exception{
				public:
					const char *what() const throw();
			};
};


#endif