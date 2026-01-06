/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 23:36:59 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/06 23:57:12 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTPResponse.hpp"

HTTPResponse::HTTPResponse(){}

HTTPResponse::HTTPResponse(const HTTPResponse &copy)
{
    *this = copy;
}

HTTPResponse &HTTPResponse::operator=(const HTTPResponse &copy)
{
    if(this != &copy)
    {
        _version = copy._version;
        _status = copy._status;
        _reason = copy._reason;
        _headers = copy._headers;
        _body = copy._body;
    }
    
    return *this;
}

HTTPResponse::~HTTPResponse(){}


void HTTPResponse::setStatus(int status, const std::string &reason)
{
    _status = status;
    _reason = reason;
}

void HTTPResponse::setHeader(const std::string &key, const std::string &value)
{
    _headers[key] = value;
}

void HTTPResponse::setBody(const std::string &body)
{
    _body = body; 
}

std::string HTTPResponse::reasonPhrase(int code)
{
    switch (code)
    {
        case 200: return "OK";
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 501: return "Not Implemented";
        default:  return "Error";
    }
}

std::string HTTPResponse::response()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << _status << " " << reasonPhrase(_status) << "\r\n";
    ss << "Content-Length: " << _body.size() << "\r\n";
    ss << "Content-Type: " << "text/html\r\n";
    ss << "\r\n";
    ss << _body;
    
    return ss.str();
}
