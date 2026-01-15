#include "../include/Response.hpp"
//------------------------------#
//			constructors		#
//------------------------------#
Response::Response():statusCode(404), body("")
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
	}
	return *this;
}
//------------------------------#
//			getters				#
//------------------------------#
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
std::string Response::reasonPhrase(int code)
{
	std::cout << "status code in creattion res: " << code << std::endl;
    switch (code)
    {
        case 200: return "OK";
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 501: return "Not Implemented";
		case 400: return "Bad Request!";
		case 413: return "Payload Too Large";
		case 431: return "Request Header Fields Too Large";
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

std::string Response::toString()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << getStatusCode() << " " << reasonPhrase(getStatusCode()) << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Content-Type: text/html\r\n";
	//ss << "Set-Cookie: yummy_cookie=chocolate\r\n";
    ss << "\r\n";
    ss << body;

    return ss.str();
}
