#include "../include/ResponseHandler.hpp"
#include "../include/Config.hpp" // to use location and config

//------------------------------#
//			constructors		#
//------------------------------#
ResponseHandler::ResponseHandler()
{
} // ResponseHandler to this request, this req has the socket fd
ResponseHandler::~ResponseHandler()
{
}
ResponseHandler::ResponseHandler(const ResponseHandler &res) // copy constructor
{
	*this = res;
}
ResponseHandler &ResponseHandler::operator=(const ResponseHandler &other)
{
	if (this != &other)
	{
		this->q = other.q;
	}
	return (*this);
}
static struct Location matchLocation(struct Config server,std::string path)
{
	struct Location location;
	size_t i = 0;
	size_t longest = 0;
	
	while (i < server.locations.size())
	{
		std::string p = server.locations[i].path;
		if (path.find(p) == 0 && p.size() > longest) // this longest say the largsest match
		{
			location = server.locations[i];
			longest = p.size();
		}
		
		i++;
	}
	return location;
}

static struct Config matchServer(const HttpRequest &req, std::vector<struct Config> servers)
{
	struct Config server;
	size_t i = 0;
	while (i < servers.size())
	{
		if (servers[i].port == req.getPortServer())
			server =  servers[i];
		i++;
	}
	return server;	
}

/* methods */
/* after fsm parsed the req,
	that signel http_req pass here and buil respons for it */
void ResponseHandler::controller(const HttpRequest &req,
	std::vector<struct Config> servers)
{
	//handleRequest(sock_fd) // req has the sockfd,
	//	: req = http_req[sock_fd]
	//		// not the list of request sinlgle request after the fsm
	//		// http_req.getPath(); //

	//		// take out server with match port
	//		// take out locatoin and send to push into queue
	
	struct Config server = matchServer(req, servers);
	if (server.empty)
	{
		std::cout << "send: 404\n";
	}
	std::cout << "what is request: " << req.getBody() << std::endl;
	std::cout << "what is path: " << req.getPath() << std::endl;
	
	struct Location location = matchLocation(server, req.getPath());
	std::cout << "path that is match: " << location.path << std::endl;
	if (location.empty)
	{
		std::cout << "send: 404\n";
	}
	//if (!methodAllowed(location, req.method))
	//{
	//send:
	//	respond 405
	//}

	//// full_path = resolvePath(location.root, req.path)

	//// switch (req.method)
	////: GET    → handleGet() POST   → handlePost() DELETE → handleDelete()

	//if (req.getMethod() == "GET")
	//	handleGet(...) else if (req.getMethod() == "POST") handlePost(...) else if (req.getMethod() == "DELETE") handleDelete(...) else respond 501;
	//send(ResponseHandler, sock_fd)
	(void)req;
	std::cout << "in controller: " << server.port << std::endl;

}