#include "../include/Server.hpp"
#include "../include/Config.hpp"
//#include "../include/ParsedData.hpp"
#include <fcntl.h>

int	main(int ac, char *argv[], char **env)
{
	//if (ac != 2)
	//{
	//	std::cout << "[USAGE] ./server config.con\n";
	//	return (1);
	//}
	(void)ac;
	(void)argv;
	(void)env;
	//Config conf("./conf"); // implicit conversion
	Config conf; // implicit conversion
	Server server(conf);
	//server.create_socket_bind();
	server.run();

	return 0;
}
