#ifndef HttpRequest_hpp
# define HttpRequest_hpp

//#include "./server.hpp"
#include <iostream>
#include <map>
#include <sstream>
#include <cerrno>
#include <algorithm> // for std::transform
#include <cctype>    // for std::tolower
#include <map>
#include <ostream>



typedef struct data{
} temp_data;

class HttpRequest
{
	private:
		// request line; first line
		std::string method;
		std::string path;
		std::string protocol;
		// header
		std::map<std::string, std::string>	header; // header["host"] : "127.0.0.1:8080"
		std::string	body;
	public:
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		/* getter */
		const std::string& getMethod() const;
		const std::string& getPath() const;
		const std::string& getProtocol() const;
		const std::map<std::string, std::string>& getHeader() const;
		std::map<std::string,std::string>& getHeader();
		const std::string& getBody () const;
		/* stetter */
		void setMethod(const std::string& _method);
		void setPath(const std::string& path);
		void setProtocol(const std::string& prot);
		void setHeader(const std::map<std::string, std::string>& header);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);


};


bool isMethod(std::string method);

//std::ostream& operator<<(std::ostream& out, HttpRequest& http);
std::ostream& operator<<(std::ostream& out, const HttpRequest& http);


#endif


//curl -v -X POST -d "name=Bob&age=22" http://127.0.0.1:4545/form
//=>
//POST /form HTTP/1.1
//Host: 127.0.0.1:4545
//User-Agent: curl/8.5.0
//Accept: */*
//Content-Length: 15
//Content-Type: application/x-www-form-urlencoded

//name=Bob&age=22

