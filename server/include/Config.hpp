#ifndef Config_hpp
# define Config_hpp

#include <iostream>
#include <vector>

struct NodeConf {
    std::string name;
	std::vector<std::string> args;
    std::vector<NodeConf> children;
};

class Config{
	private:
		int port;
		std::string host;
		struct NodeConf Node;
	public:
		Config();
		Config(const std::string& config); // create an object from config.conf
		Config(const Config& other);
		//Config& operator=(const Config& other);
		~Config();

		/* gettter */
		int getPort() const;
		const std::string& getHost() const;
};

#endif