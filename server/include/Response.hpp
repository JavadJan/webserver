#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <sstream>
#include <iostream>
#include <map>

class Response{
	private:
		int status;
		std::string body;
		std::map<std::string, std::string> header;
	public:
		Response();
		~Response();
		Response(const Response& other);
		Response& operator=(const Response& other);
		/* methods */
		std::string reasonPhrase(int status);
		std::string toString(); // to string

		/* setter */
		void setStatus(int status);
		void setBody(std::string body);
		void setHeader(std::map<std::string, std::string> header);
};
#endif