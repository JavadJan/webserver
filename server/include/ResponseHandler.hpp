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

#include "./Response.hpp" // for response to

class ResponseHandler
{
  private:
	Response res;
	std::string full_path; // helper to track macth: server|location|path|mathod
  public:
	ResponseHandler(); // response to this request, this req has the socket fd
	~ResponseHandler();
	ResponseHandler(const ResponseHandler &res);
	ResponseHandler &operator=(const ResponseHandler &other);
	// getter
	Response getResponse();

	// method controller
	void controller(const HttpRequest &req,	std::vector<struct Config> servers);
	void handelGet();
	void handlePost();
	void handleDelete();
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