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

static Location matchLocation(const Config& server, const std::string& path)
{
    Location best;
    size_t longest = 0;

    for (size_t i = 0; i < server.locations.size(); i++)
    {
        const std::string& p = server.locations[i].path;
        if (path.find(p) == 0 && p.size() > longest)
        {
            best = server.locations[i];
            longest = p.size();
        }
    }
    return best;
}



static bool find_method(const std::vector<std::string>& methods,
                        const std::string& method)
{
    return std::find(methods.begin(), methods.end(), method) != methods.end();
}

static bool methodAllowed(const Location& location, const std::string& method)
{
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        location.directive.find("allow_methods");

    if (it == location.directive.end())
	{
        return true; // no restriction = allow all
	}
	std::cout << "methods : " ;
	for (size_t i = 0; i < it->second.size(); i++)
	{
		std::cout << it->second[i] << " " ;
	}
	std::cout << std::endl;
    return find_method(it->second, method);
}

static std::string resolvePath(const std::string &req_path, const Location& location)
{
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        location.directive.find("root");

    if (it == location.directive.end() || it->second.empty())
        return ""; // let caller handle 404 / 500

    std::string root = it->second[0];

    if (req_path.find(location.path) == 0)
    {
        root += "/";
        root += req_path.substr(location.path.length());
		std::cout << "root: " << root << std::endl;
    }
    else
    {
        root += req_path;
		std::cout << "root: " << root << std::endl;
    }
    return root;
}


bool ResponseHandler::path_exist(std::string full_path)
{
	struct stat st;
    return (stat(full_path.c_str(), &st) == 0);
}


/* methods */
/* after fsm parsed the req,
	that signel http_req pass here and buil respons for it */
void ResponseHandler::controller(const HttpRequest &req, struct Config server)
{
	std::cout << "what is request: " << req.getBody() << std::endl;
	std::cout << "what is req path: " << req.getPath() << std::endl;
	
	struct Location location = matchLocation(server, req.getPath());
	std::cout << "req.path that is match: " << location.path << std::endl;
	if (location.empty)
	{
		res.setStatus(404);
		std::cout << "send: 404\n";
	}

	
		//full_path = resolvePath(req.getPath(), location); // 
	full_path = resolvePath(req.getPath(), location);
	std::cout << "full path: " << full_path << std::endl;
	if (!path_exist(full_path)) // ./tmp/www/form does not exist
	{
		res.setStatus(404);
		return;
	}

	if (!methodAllowed(location, req.getMethod()))
	{
		res.setStatus(405);
		std::cout << "send:	respond 405 " <<std::endl;
		return;
	}
		//router.get("/", (req, res)=>{
		//	res.send("hello")
		//})
		//std::cout << "allowed method: " << location.directive["allow_methods"].at(0) << std::endl;
	if (req.getMethod() == "GET")
	{
		handleGet();
	}
	else if (req.getMethod() == "POST")
		handlePost();
	else if (req.getMethod() == "DELETE")
		handleDelete();
	else 
	{
		res.setStatus(501);
		std::cout << "respond 501\n";

	}	

	

	//adjust path

	//send(ResponseHandler, sock_fd)
	(void)req;
	std::cout << "in controller: " << server.port << std::endl;

}


// 
void ResponseHandler::handleGet()
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
        // Try index.html
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
    buffer << file.rdbuf(); // has rdbuf() allowed?

    res.setStatus(200); // set the correct status
    res.setBody(buffer.str());
}

void ResponseHandler::handleDelete()
{

}
void ResponseHandler::handlePost()
{

}
void ResponseHandler::finalize(const HttpRequest& req, const Config& server)
{
    if (res.getStatus() < 400)
        return;

    // if body already set, do nothing
    if (!res.getBody().empty())
        return;

    renderErrorPage(req, server);
}
//--------------------------#
// 			getter			#
//--------------------------#
Response& ResponseHandler::getResponse()
{
	return res;
}

static void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos)
    {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static std::string intToString(int value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

void ResponseHandler::renderErrorPage(const HttpRequest &req, const Config& server)
{
    int status = res.getStatus();
	(void)req;
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        server.directives.find("error_page");

    if (it == server.directives.end())
    {
        res.setBody("Error " + intToString(status));
        return;
    }

    std::ifstream file(it->second[0].c_str());
    if (!file)
    {
        res.setBody("Error " + intToString(status));
        return;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string body = buf.str();
    replaceAll(body, "{error}", intToString(status));
    replaceAll(body, "{text}", res.reasonPhrase(status));

    res.setBody(body);
}

/* 

stat() is like asking the filesystem:

“Hey, what is this path?
	A file? A directory?
	How big is it?
	When was it modified?
	Do I have permission to read it?”



*/