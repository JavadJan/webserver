#include "../include/Server.hpp"
#include "../include/Config.hpp"
//#include "../include/ParsedData.hpp"
#include <fcntl.h>

int	main(int ac, char *argv[], char **env)
{
	if (ac != 2)
	{
		std::cout << "[USAGE] ./server config.con\n";
		return (1);
	}
	(void)ac;
	(void)argv;
	(void)env;
	//Config conf("./conf"); // implicit conversion
	//Config conf; // implicit conversion
	try
	{
		/* code */
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