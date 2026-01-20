#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP
# include "./Config.hpp"
# include "./HttpRequest.hpp"
# include "./Config.hpp"
# include <iostream>
# include <queue> 
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cerrno>       // for errno, std::strerror
#include <cstring>

#include "./Response.hpp" // for response to

class ResponseHandler
{
  private:
	Response res;
	struct Location* loc;
	std::string full_path; // helper to track macth: server|location|path|mathod
  public:
	ResponseHandler(); // response to this request, this req has the socket fd
	~ResponseHandler();
	ResponseHandler(const ResponseHandler &res);
	ResponseHandler &operator=(const ResponseHandler &other);

	void setLocation(Location* location); // setter for loc
	Location* getLocation()const; // setter for loc
	// getter
	Response& getResponse();

	void setSendBuffer(std::string str);
	void setOffset(size_t offset);
	// method controller
	void controller(const HttpRequest &req,	struct Config servers);
	bool path_exist(std::string full_path);
	void finalize(const HttpRequest &req, const Config& servers);
	std::vector<char*> buildCGIEnv(const HttpRequest &req, const Config& servers);
	void handleGet();
	void handlePost(const HttpRequest &req, const Config &server);
	void handleDelete();
	void handleCGI(const HttpRequest &req, const Config &server);
	void handleUpload(const HttpRequest &req, const Config &server);
	bool isCGI();
	class ResException: public std::exception{
		public:
		const char* what() const throw();
	};

	// send error page
	void renderErrorPage(const HttpRequest &req, const Config& server);
};

#endif

/* controller: 
Match server (host:port)
↓
Match location (URI prefix)
↓
Check allowed methods
↓
Resolve filesystem path

----------------------------
config.conf -> parseConfig() -> server(){extract data and use} -> handler response -> a response obj ->


*/