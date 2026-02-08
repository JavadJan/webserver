#ifndef RESPONSE_HPP
# define RESPONSE_HPP

#include <sstream>
#include <iostream>
#include <map>

class Response{
	private:
		int statusCode;
		std::string body;
		std::map<std::string, std::string> header;
<<<<<<< HEAD
		bool autoindex;
		std::string ContentType;
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
	public:
		Response();
		~Response();
		Response(const Response& other);
		Response& operator=(const Response& other);
		/* methods */
		std::string reasonPhrase(int status);
		std::string toString(); // to string
<<<<<<< HEAD

		int getStatusCode() const;
		bool getAutoindex() const;
		std::string getBody() const;
		std::map<std::string, std::string> getHeader() const;
		std::string getContType() const;

		/* setter */
		void setStatusCode(int status);
		void setContType(std::string type);
		void setAutoindex(bool index);
=======
		int getStatusCode() const;
		std::string getBody() const;
		std::map<std::string, std::string> getHeader() const;

		/* setter */
		void setStatusCode(int status);
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
		void setBody(std::string body);
		void setHeader(std::map<std::string, std::string> header);
		void setHeader(const std::string& key, const std::string& value);
};
#endif