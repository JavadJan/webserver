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
#define BUG false
// server{
//	listen 8080;
//	route /docs/www;

//	#route
//	location /
//	{
		//method GET,DELETE
	
//	}
//	location /doc
//	{
		//method GET,DELETE
	
//	}
//}
//directive[method] = "GET, DELETE";
struct	Location
{
	std::string path;
	std::map<std::string, std::vector<std::string> > directive;
	bool empty;
	Location();
	~Location();
	Location(const struct Location& other);
	Location& operator=(const struct Location& other); 
};

struct	Config
{
	std::string port;
	std::string host;
	std::map<std::string, std::vector<std::string> > directives;
	std::vector<struct	Location> locations;
	bool empty;
	Config();
};

std::vector<struct Config> parseConfig(const char* conf);

#endif