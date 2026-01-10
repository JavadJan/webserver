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

struct Config;
//enum	STATE {	REQ_LINE, HEADER, BODY, DONE, ERROR	};

class HttpRequest
{
	public:
		std::string sendBuffer;
		size_t sendOffset;
		enum STATE { REQ_LINE, HEADER, BODY, DONE, ERROR, SENDING };
		HttpRequest();
		~HttpRequest();
		HttpRequest(const HttpRequest& other);
		HttpRequest& operator=(const HttpRequest& other);

		/* getter */
		STATE getState() const;
		const std::string& getMethod() const;
		const std::string& getPath() const;
		const std::string& getProtocol() const;
		const std::map<std::string, std::string>& getHeader() const;
		std::map<std::string,std::string>& getHeader();
		const std::string& getBody () const;
		const std::string& getBuffer () const;
		size_t getContetn () const;
		int getCleintSocket () const;
		std::string getPortServer() const;
		bool getConnectionState();
		//Config * getServerConfig();
		const Config* getServerConfig() const;
		int	getStatusCode() const;
		/* stetter */
		void setMethod(const std::string& _method);
		void setPath(const std::string& path);
		void setProtocol(const std::string& prot);
		void setHeader(const std::map<std::string, std::string>& header);
		void setHeader(const std::string& key, const std::string& value);
		void setBody(const std::string& body);
		void setContent(size_t len);
		void setClientSocket(int fd);
		void setPortServer(std::string port);
		void setServerConfig(Config *server);
		void setStatusCode(int status_code); // trace the status code for req valid
		void clearBuffer();
		
		void setState(enum STATE state);
		void setCloseConnection(bool cstate);
		void appendBuffer(std::string chunk, int bytes_read);
		void eraseBuffer(size_t start, size_t end);

		void resetForNextRequest();
	private:
		STATE state;
		std::string recvBuffer;
		std::string portServer;
		// request line; first line
		std::string method;
		std::string path;
		std::string protocol;
		int sock_fd_cleint;
		// header
		std::map<std::string, std::string>	header; // header["host"] : "127.0.0.1:8080"
		size_t conten_len;
		std::string	body;

		struct Config* server;
		bool connection_close;
		int statusCode; // trace the valid request
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

