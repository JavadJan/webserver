#ifndef RESPONSEHANDLER_HPP
# define RESPONSEHANDLER_HPP
# include "./Config.hpp"
# include "./HttpRequest.hpp"
# include <iostream>
# include <queue> 

class ResponseHandler
{
  private:
	std::queue<std::string> q; // helper to track macth: server|location|path|mathod
  public:
	ResponseHandler(); // response to this request, this req has the socket fd
	~ResponseHandler();
	ResponseHandler(const ResponseHandler &res);
	ResponseHandler &operator=(const ResponseHandler &other);

	// method controller
	void controller(const HttpRequest &req, std::vector<struct Config> servers);
	void handelGet();
	void handelPost();
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



*/