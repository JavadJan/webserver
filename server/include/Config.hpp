#ifndef Config_hpp
# define Config_hpp

#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <cstdlib>      // std::atoi

class Config{
	private:
		int port;
		std::string host;
	public:
		Config();
		Config(const std::string& config); // create an object from config.conf
		Config(const Config& other);
		Config& operator=(const Config& other);
		~Config();

		/* gettter */
		int getPort() const;
		const std::string& getHost() const;
};

#endif