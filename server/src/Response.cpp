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
		this->header = header;
	}
	return *this;
}


//------------------------------#
//			methods				#
//------------------------------#
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
std::string Response::toString(std::string http_version)
{
	// convert to a string with those attribute
}