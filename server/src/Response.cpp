/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mkhavari <mkhavari@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 17:54:01 by asemykin          #+#    #+#             */
/*   Updated: 2026/04/15 21:34:34 by mkhavari         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../include/Response.hpp"
#include "../include/Config.hpp"

//------------------------------#
//			constructors		#
//------------------------------#
Response::Response():statusCode(404), body(""), autoindex(false)
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
		this->autoindex = other.autoindex;
	}
	return *this;
}
//------------------------------#
//			getters				#
//------------------------------#
bool Response::getAutoindex() const
{
	return this->autoindex;
}



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
void Response::setAutoindex(bool autoIndex)
{
	this->autoindex = autoIndex;
} 
std::string Response::reasonPhrase(int code)
{
	if(BUG)
	{std::cout << "status code in creattion res: " << code << std::endl;}
    switch (code)
    {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
		case 301: return "Moved Permanently";
		case 400: return "Bad Request!";
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 411: return "Length Required";
        case 501: return "Not Implemented";
		case 413: return "Payload Too Large";
		case 431: return "Request Header Fields Too Large";
		case 423: return "Busy, Locked";
		case 504: return "Gateway Timeout";
		case 503: return "Service Unavailable";
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

std::string Response::getContType() const
{
	return this->ContentType;
}

void Response::setContType(std::string s)
{
	this->ContentType = s;
}

std::string Response::toString()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << getStatusCode() << " " << reasonPhrase(getStatusCode()) << "\r\n";
    
	
	for(std::map<std::string, std::string>::iterator it = header.begin(); it != header.end(); it++)
		ss << it->first << ": " << it->second << "\r\n";
	
	ss << "Content-Length: " << body.size() << "\r\n";
    
	if(!ContentType.empty())
		ss << "Content-Type: " << getContType() << "\r\n";
	ss << "Set-Cookie: yummy_cookie=chocolate\r\n";
    ss << "\r\n";
	ss << body;

    return ss.str();
}
