#include "../include/Server.hpp"
#include "../include/Config.hpp"
//#include "../include/ParsedData.hpp"
#include <fcntl.h>



int	main(int ac, char *argv[])
{
	if (ac != 2)
	{
		std::cout << "[USAGE] ./server config.con\n";
		return (1);
	}
	
	// prevent to ctrl +Z to work, prevent Zombie
	struct sigaction sa_tstp;
	sa_tstp.sa_handler = SIG_IGN;
	sigemptyset(&sa_tstp.sa_mask);
	sa_tstp.sa_flags = 0;
	sigaction(SIGTSTP, &sa_tstp, NULL);



	struct sigaction sa;
	sa.sa_handler = Server::signal_handler; // call this function
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if ((sigaction(SIGINT, &sa, NULL) == -1) ||
		 sigaction(SIGTERM, &sa, NULL) == -1)
	{
		std::cerr << "Failed to set signal handlers: " << strerror(errno) << std::endl;
		return 1;
	}
	try
	{
		std::vector<struct Config> serversConfig = parseConfig(argv[1]);
		std::cout << serversConfig[0].port << std::endl;
		
		Server server(serversConfig);
		server.run();
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << '\n';
	}
	
	//server.create_socket_bind();

	return 0;
}

/* 
	
*/