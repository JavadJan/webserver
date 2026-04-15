/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseHandler.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:54:03 by asemykin          #+#    #+#             */
/*   Updated: 2026/02/10 17:54:04 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/ResponseHandler.hpp"
#include "../include/Config.hpp" // to use location and config
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <cstdlib>
#include <dirent.h>

//------------------------------#
//			constructors		#
//------------------------------#
ResponseHandler::ResponseHandler()
:res(),
loc(),
full_path(),
root(),
cgiScript(),
headerCGI(),
bodyCGI(),
has_loc(false)
{

}

// ResponseHandler to this request, this req has the socket fd
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


void ResponseHandler::setLocation(const Location& l)
{
    loc = l;        // copy by value
    has_loc = true;
}

const Location& ResponseHandler::getLocation() const
{
    return loc;
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
        if (path.find(p) == 0 && p.size() >= longest)
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
    if(BUG)
	{std::cout << "methods : " ;
	for (size_t i = 0; i < it->second.size(); i++)
	{
		std::cout << it->second[i] << " " ;
	}
	std::cout << std::endl;}
    return find_method(it->second, method);
}

static std::string resolvePath(const std::string &req_path, const Location& location)
{
    std::map<std::string, std::vector<std::string> >::const_iterator it =
        location.directive.find("root");

    if (it == location.directive.end() || it->second.empty())
        return ""; // let caller handle 404 / 500

    return it->second[0] + req_path;
    // std::string root = it->second[0];

    // if (req_path.find(location.path) == 0)
    // {
    //     root += "/";
    //     root += req_path.substr(location.path.length());
	// 	std::cout << "root: " << root << std::endl;
    // }
    // else
    // {
    //     root += req_path;
	// 	std::cout << "root: " << root << std::endl;
    // }
    // return root;
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
    if(BUG)
	{std::cout << "what is request: " << req.getBody() << std::endl;
	std::cout << "what is req path: " << req.getPath() << std::endl;}
	if (req.getPath().empty())
	{
		res.setStatusCode(400);
		return ;
	}
	struct Location location = matchLocation(server, req.getPath());
    if(BUG)
	{std::cout << "req.path that is match: " << location.path << std::endl;}
	// if (location.empty)
	// {
	// 	res.setStatusCode(404);
	// 	std::cout << "send: 404\n";
	// 	return ;
	// }
	
	this->setLocation(location);
	
		//full_path = resolvePath(req.getPath(), location); // 
	full_path = resolvePath(req.getPath(), location);
	
    if(req.getPath() == "/redirect")
    {
        res.setStatusCode(301);
        res.setHeader("Location", "/");
        if(BUG)
        {std::cout << "redirect:	respond 301 " << std::endl;}
        return;
    }

	if (!methodAllowed(location, req.getMethod()))
	{
		res.setStatusCode(405);
        res.setContType(getMimeType(".html"));
        if(BUG)
		{std::cout << "send:	respond 405 " <<std::endl;}
		return;
	}

	if (req.getMethod() == "GET")
	{
		if (isCGI())
			handleCGI(req, server);
		else
			handleGet(req);
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
        if(BUG)
		{std::cout << "respond 501\n";}
		return ;
	}	
}

static bool autoIndex(struct Location &loc)
{
	//// reamin
	std::map<std::string, std::vector<std::string> >::const_iterator obj = loc.directive.find("autoindex");
	if (obj != loc.directive.end() && !obj->second.empty() && obj->second[0] == "on")
	{
		return true; 
	}	
	//confServer.locations.
	return false;
}

std::string ResponseHandler::generateAutoindex(const std::string& dirpath, const std::string& urlpath)
{
    DIR* dir = opendir(dirpath.c_str());
    if (!dir)
	{
		res.setStatusCode(403);
		res.setBody("Forbidden");
		return "<h1>Forbidden</h1>";
	}

    std::stringstream html;

    html << "<html><head><title>Index of " << urlpath << "</title></head><body>";
    html << "<h1>Index of " << urlpath << "</h1><hr><pre>";

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;

        // skip . and ..
        if (name == ".")
            continue;

        std::string full = dirpath + "/" + name;

        struct stat st;
        stat(full.c_str(), &st);

        if (S_ISDIR(st.st_mode))
            name += "/";

        // html << "<a href=\"" << name << "\">" << name << "</a>\n";
        std::string urlpathSlash = urlpath;
        if (urlpathSlash[urlpathSlash.size() - 1] != '/')
            urlpathSlash += '/';

        html << "<a href=\"" << urlpathSlash << name << "\">" << name << "</a>\n";
    }

    html << "</pre><hr></body></html>";

    closedir(dir);
    return html.str();
}

std::string getMimeType(const std::string& path)
{
    size_t dot = path.rfind('.');
    if (dot == std::string::npos)
        return "application/octet-stream";

    std::string ext = path.substr(dot + 1);

    if (ext == "html" || ext == "htm") return "text/html";
    if (ext == "css") return "text/css";
    if (ext == "js") return "application/javascript";
    if (ext == "png") return "image/png";
    if (ext == "jpg" || ext == "jpeg") return "image/jpeg";
    if (ext == "gif") return "image/gif";
    if (ext == "txt") return "text/plain";

    return "application/octet-stream"; // default binary
}

// 
void ResponseHandler::handleGet(const HttpRequest& req)
{
	struct stat st;
    if(BUG)
	{std::cout << "full path: " << full_path << std::endl;}
	if (!path_exist(full_path)) // ./tmp/www/form does not exist
	{
        if(BUG)
		{std::cout << "PATH does not exist: " << res.getStatusCode();}
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
		if (stat(index.c_str(), &st) == 0 && S_ISREG(st.st_mode))
			full_path = index;
		else if (autoIndex(loc))
		{
			std::string html = generateAutoindex(full_path, req.getPath());
			res.setAutoindex(true); // special status just for autoindex
			res.setStatusCode(200); // in response generate the file, not write just send a tem file
			res.setBody(html);
			return ;
		}
		else
		{
			res.setStatusCode(403);
			res.setBody("Forbidden");
			return;

		}
    }
	//regular file
	std::ifstream file(full_path.c_str(), std::ios::binary); // binary for image
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
	res.setContType(getMimeType(full_path));
    if(BUG)
    	std::cout << "CONTENT TYPE: " << req.getContentType() << std::endl;
}

void ResponseHandler::handleDelete()
{
    struct stat st;

    // Check if path exists
    if (stat(full_path.c_str(), &st) == -1)
    {
        res.setStatusCode(404); // Not Found
        return;
    }

    // Reject directories
    if (S_ISDIR(st.st_mode))
    {
        res.setStatusCode(403); // Forbidden
        return;
    }

    // Check write permission
    if (access(full_path.c_str(), W_OK) == -1)
    {
        if (errno == EACCES || errno == EPERM)
        {
            res.setStatusCode(403); // Forbidden
            return;
        }
    }

    // Attempt deletion
    if (remove(full_path.c_str()) == 0)
    {
        res.setStatusCode(204); // No Content
        return;
    }

    // Map errno to correct HTTP status
    switch (errno)
    {
        case EACCES:
        case EPERM:
        case EROFS:
            res.setStatusCode(403); // Forbidden
            break;
        case ENOENT:
            res.setStatusCode(404); // Not Found
            break;
        case EBUSY:
            res.setStatusCode(423); // BUSY 
            break;
        default:
            res.setStatusCode(500); // Internal Server Error
            break;
    }
}


static bool uploadEnabled(const Location &loc)
{
    if (loc.path.empty())
        return false;
    //// reamin
    std::map<std::string, std::vector<std::string> >::const_iterator obj = loc.directive.find("allow_upload");
    if (obj != loc.directive.end() && !obj->second.empty() && obj->second[0] == "on")
    {
        return true; 
    }	
    //confServer.locations.
    return false;
}
void ResponseHandler::handlePost(const HttpRequest &req, const Config &server)
{
    // CGI takes priority
    if (isCGI())
    {
        handleCGI(req, server);
        return;
    }

    // Upload enabled?
    if (!uploadEnabled(this->loc))
    {
        res.setStatusCode(403);
        res.setContType(getMimeType(".html"));
        return;
    }

    handleUpload(req, server);
    return;

    // Check filesystem -> absolute path
    struct stat st;
    if (stat(full_path.c_str(), &st) != 0)
    {
        // Path does not exist
        res.setStatusCode(404);
        res.setContType(getMimeType(".html"));
        return;
    }

    // 4. If it's a regular file → POST not allowed
    if (S_ISREG(st.st_mode))
    {
        res.setStatusCode(405); // Method Not Allowed
        res.setContType(getMimeType(".html"));
        return;
    }

    // 5. If it's a directory
    if (S_ISDIR(st.st_mode))
    {
        // Try index.html
        std::string index = full_path + "/index.html";
        if (stat(index.c_str(), &st) == 0 && S_ISREG(st.st_mode))
        {
            full_path = index; // serve index.html
			std::ifstream file(full_path.c_str(), std::ios::binary); // binary for image
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
			res.setContType(getMimeType(full_path));
			//res.setStatusCode(200);
            return;
        }

        // Autoindex NEVER applies to POST
        if (autoIndex(loc))
        {
            res.setStatusCode(405); // Method Not Allowed
            res.setContType(getMimeType(".html"));
            return;
        }

        // Directory exists but no index → POST not allowed
        res.setStatusCode(405);
        res.setContType(getMimeType(".html"));
        return;
    }

    // 6. Fallback
    res.setStatusCode(404);
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

    if(BUG)
	{std::cout << "status code renderErrorPage: " << res.getStatusCode() << std::endl;}
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