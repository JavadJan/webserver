#ifndef Config_hpp
# define Config_hpp

#include <iostream>

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