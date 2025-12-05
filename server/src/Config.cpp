#include "../include/Config.hpp"
Config::Config():port(4242), host("127.0.0.1")
{

}
Config::Config(const std::string& conf)
{
	// here parse the config file
	// use json parser
	(void)conf;

}
Config::Config(const Config& other): port(other.port), host(other.host)
{

}

Config::~Config(){}

/* getter */
int Config::getPort() const
{
	return (this->port);
}
const std::string& Config::getHost() const
{
	return (this->host);
}