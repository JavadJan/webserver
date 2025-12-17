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
		/* setter */
		void setStatus(int status);
		void setBody(std::string body);
		void setHeader(std::map<std::string, std::string> header);
		std::string toString(std::string http_version); // to string
};
#endif