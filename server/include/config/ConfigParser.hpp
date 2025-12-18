#ifndef ConfigParser_hpp
# define ConfigParser_hpp

#include <iostream>
#include <fstream>      // std::ifstream
#include <sstream>      // std::istringstream
#include <cstdlib>      // std::atoi
#include <vector>
#include "Token.hpp"
#include "ServerConfig.hpp"
#include "LocationConfig.hpp"


class ConfigParser{
	private:
		std::vector<Token> _tokens;
		size_t _pos;

	public:
		ConfigParser(const std::string &filename);
		~ConfigParser(){};

		std::vector<ServerConfig> parse();
	
	private:
		void tokenize(const std::string& content);

		Token& peek();
		Token& next();

		bool accept(TokenType type);
		void expect(TokenType type);

		ServerConfig parseServer();
		LocationConfig parseLocation();
		
};

#endif