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

void ResponseHandler::setLocation(Location *location)
{
	this->loc = location;
}
Location* ResponseHandler::getLocation() const
{
	return this->loc;
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
	if (req.getPath().empty())
	{
		res.setStatusCode(400);
		return ;
	}
	struct Location location = matchLocation(server, req.getPath());
	std::cout << "req.path that is match: " << location.path << std::endl;
	if (location.empty)
	{
		res.setStatusCode(404);
		std::cout << "send: 404\n";
		return ;
	}
	
	this->setLocation(&location);
	
		//full_path = resolvePath(req.getPath(), location); // 
	full_path = resolvePath(req.getPath(), location);
	

	if (!methodAllowed(location, req.getMethod()))
	{
		res.setStatusCode(405);
		std::cout << "send:	respond 405 " <<std::endl;
		return;
	}

	if (req.getMethod() == "GET")
	{
		if (isCGI())
			handleCGI(req, server);
		else
			handleGet();
		return ;
	}
	else if (req.getMethod() == "POST")
	{
		handlePost(req, server);
		return ;	
	}
	else if (req.getMethod() == "DELETE")
	{
		handleDelete();
		return ;	
	}
	else 
	{
		res.setStatusCode(501);
		std::cout << "respond 501\n";
		return ;
	}	
}


// 
void ResponseHandler::handleGet()
{
	struct stat st;
	std::cout << "full path: " << full_path << std::endl;
	if (!path_exist(full_path)) // ./tmp/www/form does not exist
	{
		std::cout << "PATH does not exist: " << res.getStatusCode();
		res.setStatusCode(404);
		return;
	}
	if (stat(full_path.c_str(), &st) == -1) // if not found
	{
		res.setStatusCode(404);
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
            res.setStatusCode(403);
            res.setBody("Forbidden");
            return;
        }
    }
	//regular file
	std::ifstream file(full_path.c_str()); // here read why?
    if (!file)
    {
        res.setStatusCode(403);
        res.setBody("Forbidden");
        return;
    }
	std::stringstream buffer;
    buffer << file.rdbuf(); // has rdbuf() allowed?

    res.setStatusCode(200); // set the correct status
    res.setBody(buffer.str());
}

void ResponseHandler::handleDelete()
{

}

static bool uploadEnabled(struct Location *loc)
{
	std::map<std::string, std::vector<std::string> >::const_iterator obj = loc->directive.find("allow_upload");
	if (obj->second[0] == "on")
	{
		return true;
	}	
	//confServer.locations.
	return false;
}

//static long long maxBodySize(struct Config serverConf)
//{
//	long long body_size = atoll(serverConf.directives["max_body_size"][0].c_str());
//	return body_size;
//}

//static std::string contetnType(HttpRequest req)
//{
//	std::string multiPart = req.getHeader()["Content-Type"];
//	return multiPart;
//}




void ResponseHandler::handleUpload(const HttpRequest &req, const Config &server)
{
	(void)req;
	(void)server;

}

void ResponseHandler::handlePost(const HttpRequest &req, const Config &server)
{
	(void)server;
	(void)req;
    // CGI? this server block cover the cgi?
    if (isCGI())
    {
		std::cout << "server side is CGI\n";
        handleCGI(req, server);
        return;
    }
	std::cout << "it is not CGI\n";

    // Upload?
    if (uploadEnabled(this->loc))
    {
        handleUpload(req, server);
        return;
    }
	
    // Static file → POST not allowed
    struct stat st;
    if (stat(full_path.c_str(), &st) == 0 && S_ISREG(st.st_mode))
    {
        res.setStatusCode(405);
        return;
    }

    // 4. Not found
    res.setStatusCode(404);
	std::cout << "AHHH here the status code has changed to 404 in handle post\n";
}



void ResponseHandler::finalize(const HttpRequest& req, const Config& server)
{
    if (res.getStatusCode() < 400)
        return;

    // if body already set, do nothing
    if (!res.getBody().empty())
        return;

    renderErrorPage(req, server);
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
	// 
    int code = req.getStatusCode();
	if (code != 0)
		res.setStatusCode(code);

	std::cout << "status code renderErrorPage: " << res.getStatusCode() << std::endl;
	(void)req;
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        server.directives.find("error_page");

	// did not find error page in server config
    if (it == server.directives.end())
    {
        res.setBody("Error " + intToString(res.getStatusCode()));
        return;
    }

	// did not find the path for error page
    std::ifstream file(it->second[0].c_str());
    if (!file)
    {
        res.setBody("Error " + intToString(res.getStatusCode()));
        return;
    }

    std::stringstream buf;
    buf << file.rdbuf();

    std::string body = buf.str();
    replaceAll(body, "{error}", intToString(res.getStatusCode()));
    replaceAll(body, "{text}", res.reasonPhrase(res.getStatusCode()));

    res.setBody(body);
}


/* ----------------------------------------- */
/* 											 */
/* 				GETTER						 */
/* 											 */
/* ----------------------------------------- */
const std::string& ResponseHandler::getRoot() const
{
	return (this->root);
}
const std::string& ResponseHandler::getCGIScript() const
{
	return (this->root);
}

Response& ResponseHandler::getResponse()
{
	return res;
}
/* ----------------------------------------- */
/* 											 */
/* 				SETTER						 */
/* 											 */
/* ----------------------------------------- */
void ResponseHandler::setRoot(const std::string& _root)
{
	this->root = _root;
}
void ResponseHandler::setCGIScript(const std::string& _cgi)
{
	this->cgiScript = _cgi;
}




/* 

stat() is like asking the filesystem:

“Hey, what is this path?
	A file? A directory?
	How big is it?
	When was it modified?
	Do I have permission to read it?”



*/