#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <algorithm>
# include <fstream>
# include <iostream>
# include <map>
# include <stack>
# include <stdexcept>
# include <sstream>
# include <vector>

// server{
//	listen 8080;
//	route /docs/www;

//	#route
//	location /
//	{

//	}
//}
struct	Location
{
	std::string path;
	std::map<std::string, std::string> directive;
};

struct	Config
{
	int	port;
	std::string host;
	std::map<std::string, std::string> directives;
	std::vector<struct	Location> locations;
};

std::vector<struct Config> parseConfig(const char* conf);

#endif