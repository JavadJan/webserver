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

static std::string resolvePath(const std::string &req_path, struct Location location)
{
	// root is in config
    std::string root = location.directive["root"];
	std::cout << "root: " << root << std::endl;
	
	if (req_path.find(location.path) == 0)
	{
		root += "/";
		root += req_path.substr(location.path.length()); // ./tmp/www + /form
		std::cout << "root: " << root << std::endl;
		
	}
    else
	{
		root += req_path;
		std::cout << "root: " << root << std::endl;

	}

    return root;
}


/* methods */
/* after fsm parsed the req,
	that signel http_req pass here and buil respons for it */
void ResponseHandler::controller(const HttpRequest &req,
	std::vector<struct Config> servers)
{
	struct Config server = matchServer(req, servers);
	if (server.empty)
	{
		res.setStatus(404);
		std::cout << "send: 404\n";
	}
	
	std::cout << "what is request: " << req.getBody() << std::endl;
	std::cout << "what is req path: " << req.getPath() << std::endl;
	
	struct Location location = matchLocation(server, req.getPath());
	std::cout << "req.path that is match: " << location.path << std::endl;
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
	else // so if methods allowd 
	{
		full_path = resolvePath(req.getPath(), location); // 
		std::cout << "allowed method: " << location.directive["allow_methods"] << std::endl;
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
	}

	//adjust path

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
	//regular file
	std::ifstream file(full_path.c_str()); // here read why?
    if (!file)
    {
        res.setStatus(403);
        res.setBody("Forbidden");
        return;
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