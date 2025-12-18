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
		this->full_path = other.full_path;
	}
	return (*this);
}

//------------------------------#
//			methods, main		#
//------------------------------#
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

static bool methodAllowed(struct Location location, std::string method)
{
	size_t found = location.directive["allow_methods"].find(method);
	if (found != std::string::npos)
		return true;
	
	return false;
};

static std::string resolvePath(const std::string &req_path)
{
    std::string full = "./tmp/www";
    // Remove location prefix
    full += req_path;
    return full;
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
		res.setStatus(404);
		std::cout << "send: 404\n";
	}
	
	std::cout << "what is request: " << req.getBody() << std::endl;
	std::cout << "what is path: " << req.getPath() << std::endl;
	
	struct Location location = matchLocation(server, req.getPath());
	std::cout << "path that is match: " << location.path << std::endl;
	if (location.empty)
	{
		res.setStatus(404);
		std::cout << "send: 404\n";
	}

	if (!methodAllowed(location, req.getMethod()))
	{
		res.setStatus(405);
		
		std::cout << "send:	respond 405 " <<std::endl;
	}
	else
		std::cout << "allowed method: " << location.directive["allow_methods"] << std::endl;

	//adjust path
	full_path = resolvePath(req.getPath());

	if (req.getMethod() == "GET")
		handelGet();
	else if (req.getMethod() == "POST")
		handlePost();
	else if (req.getMethod() == "DELETE")
		handleDelete();
	else 
	{
		res.setStatus(501);
		std::cout << "respond 501\n";

	}	
	//send(ResponseHandler, sock_fd)
	(void)req;
	std::cout << "in controller: " << server.port << std::endl;

}


// 
void ResponseHandler::handelGet()
{
	struct stat st;
	if (stat(full_path.c_str(), &st) == -1) // if not found
	{
		res.setStatus(404);
		res.setBody("Not Found");
		return ;
	}
	//regular file
	std::ifstream file(full_path.c_str());
    if (!file)
    {
        res.setStatus(403);
        res.setBody("Forbidden");
        return;
    }
	
	// directory
	if (S_ISDIR(st.st_mode))
    {
        // Try index
        std::string index = full_path + "/index.html";
        if (stat(index.c_str(), &st) == 0)
            full_path = index;
        else
        {
            res.setStatus(403);
            res.setBody("Forbidden");
            return;
        }
    }
	std::stringstream buffer;
    buffer << file.rdbuf();

    res.setStatus(200);
    res.setBody(buffer.str());
}

void ResponseHandler::handleDelete()
{
	
}
void ResponseHandler::handlePost()
{

}

//--------------------------#
// 			getter			#
//--------------------------#

Response ResponseHandler::getResponse()
{
	return res;
}


/* 

stat() is like asking the filesystem:

“Hey, what is this path?
	A file? A directory?
	How big is it?
	When was it modified?
	Do I have permission to read it?”

*/