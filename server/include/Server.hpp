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

#define PORT 4242

class Config;
class HttpRequest;

class Server{
	private:
		int _port;
		struct sockaddr_in server_addr, client_addr;
		socklen_t client_len;
		char buffer[1024];
		int server_fd;
		int client_fd;

		/* for non-blocking */
		std::vector<pollfd> poll_fds;
		int poll_count; 
		void accept_new_connection();
		void add_to_poll_fds(int new_fd);
		void del_from_poll_fds(int i);
		void read_data_from_socket(int i, HttpRequest &request); // parsing heppen here
		int create_socket_bind();
		public:
			Server(Config config);
			void run();
			class ExceptionServer: public std::exception{
				public:
					const char *what() const throw();
			};
};

#endif