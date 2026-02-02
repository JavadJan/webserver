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
	
	struct sigaction sa;
	sa.sa_handler = Server::signal_handler; // call this function
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	if ((sigaction(SIGINT, &sa, NULL) == -1) || sigaction(SIGTERM, &sa, NULL) == -1)
	{
		std::cerr << "Failed to ignore SIFINT: " << strerror(errno) << std::endl;
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