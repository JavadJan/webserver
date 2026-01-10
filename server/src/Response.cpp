#include "../include/Response.hpp"
//------------------------------#
//			constructors		#
//------------------------------#
Response::Response():status(404), body("")
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
		this->status = other.status;
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
    switch (code)
    {
        case 200: return "OK";
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 501: return "Not Implemented";
		case 400: return "Bad Request!";
        default:  return "Error";
    }
}

void Response::setStatus(int status)
{
	this->status = status;
}
void Response::setBody(std::string body)
{
	this->body = body;
}
void Response::setHeader(std::map<std::string, std::string> header)
{
	this->header = header;
}

int Response::getStatus() const {
	return this->status;
}

std::string Response::toString()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << getStatus() << " " << reasonPhrase(getStatus()) << "\r\n";
    ss << "Content-Length: " << body.size() << "\r\n";
    ss << "Content-Type: text/html\r\n";
	//ss << "Set-Cookie: yummy_cookie=chocolate\r\n";
    ss << "\r\n";
    ss << body;

    return ss.str();
}
