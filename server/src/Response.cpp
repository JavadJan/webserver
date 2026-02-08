#include "../include/Response.hpp"
//------------------------------#
//			constructors		#
//------------------------------#
<<<<<<< HEAD
Response::Response():statusCode(404), body(""), autoindex(false)
=======
Response::Response():statusCode(404), body("")
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
{

} // response to this request, this req has the socket fd
Response::~Response()
{
}
Response::Response(const Response &res) // copy constructor
{
	*this = res;
}
Response &Response::operator=(const Response &other)
{
	if (this != &other)
	{
		this->statusCode = other.statusCode;
		this->body = other.body;
		this->header = other.header;
<<<<<<< HEAD
		this->autoindex = other.autoindex;
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
	}
	return *this;
}
//------------------------------#
//			getters				#
//------------------------------#
<<<<<<< HEAD
bool Response::getAutoindex() const
{
	return this->autoindex;
}



=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
std::string Response::getBody() const 
{
	return this->body;
}

std::map<std::string, std::string> Response::getHeader() const
{
	return this->header;
}

//------------------------------#
//			methods				#
//------------------------------#
<<<<<<< HEAD
void Response::setAutoindex(bool autoIndex)
{
	this->autoindex = autoIndex;
} 
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
std::string Response::reasonPhrase(int code)
{
	std::cout << "status code in creattion res: " << code << std::endl;
    switch (code)
    {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
		case 400: return "Bad Request!";
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 411: return "Length Required";
        case 501: return "Not Implemented";
		case 413: return "Payload Too Large";
		case 431: return "Request Header Fields Too Large";
<<<<<<< HEAD
		case 423: return "Busy, Locked";
		case 504: return "Gateway Timeout";
		case 503: return "Service Unavailable";
=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
        default:  return "Error";
    }
}

void Response::setStatusCode(int statusCode)
{
	this->statusCode = statusCode;
}
void Response::setBody(std::string body)
{
	this->body = body;
}
void Response::setHeader(std::map<std::string, std::string> header)
{
	this->header = header;
}
void Response::setHeader(const std::string& key, const std::string& value)
{
	this->header[key] = value;
} 
int Response::getStatusCode() const {
	return this->statusCode;
}

<<<<<<< HEAD
std::string Response::getContType() const
{
	return this->ContentType;
}

void Response::setContType(std::string s)
{
	this->ContentType = s;
}

=======
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109
std::string Response::toString()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << getStatusCode() << " " << reasonPhrase(getStatusCode()) << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
<<<<<<< HEAD
    ss << "Content-Type: ";
	ss << getContType();
	ss << "\r\n";
	//ss << "Set-Cookie: yummy_cookie=chocolate\r\n";
    ss << "\r\n";
	ss << body;
=======
    ss << "Content-Type: text/html\r\n";
	//ss << "Set-Cookie: yummy_cookie=chocolate\r\n";
    ss << "\r\n";
    ss << body;
>>>>>>> 43a25c45afca9c9962f9fd388bbdbc5365af5109

    return ss.str();
}
