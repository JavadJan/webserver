/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.cpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 23:36:59 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/18 17:37:54 by asemykin         ###   ########.fr       */
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
    
    std::stringstream ss;
    ss << _body.size();
    setHeader("Content-Length", ss.str());
}

std::string HTTPResponse::reasonPhrase(int code)
{
    switch (code)
    {
        case 200: return "OK";
        
        case 404: return "Not Found";
        case 403: return "Forbidden";
        case 405: return "Method Not Allowed";
        case 411: return "Length Required";
        
        case 500: return "Not Implemented";
        case 501: return "Not Implemented";

        case 600: return "Temp Error code";
        default:  return "Error";
    }
}

std::string HTTPResponse::response()
{
    std::stringstream ss;

    ss << "HTTP/1.1 " << _status << " " << reasonPhrase(_status) << "\r\n";
    
    std::map<std::string, std::string>::iterator it = _headers.begin();
    while(it != _headers.end())
    {
        ss << it->first << ": " << it->second << "\r\n";
        it++;
    }

    ss << "\r\n";
    ss << _body;
    
    return ss.str();
}

std::string HTTPResponse::getHeaderType(const std::string &key)
{
    if(_headers.find(key) == _headers.end())
    {
        std::cout << "DOES NOT EXISTS" << std::endl;
        return "";
    }
    return _headers[key];
}

void HTTPResponse::setErrorResponse(int status, std::string error_page)
{
    setStatus(status, reasonPhrase(status));
    
    std::string path = error_page;
    if(!path.empty())
    {
        int fd = open(path.c_str(), O_RDONLY);
        if(fd >= 0)
        {
            std::string body;
            char buffer[BUFFERSIZE];
            ssize_t bytes;
            while(1)
            {
                bytes = read(fd, buffer, sizeof(buffer));
                if(bytes <= 0)
                    break;
                body.append(buffer, bytes);
            }
            close(fd);

            setBody(body);
            
            std::string sfx = MIME().getSuffix(path);
            setHeader("Content-Type", MIME().getMimeType(sfx));
        }
        else
        {
            std::stringstream ss;
            ss << status;
            setBody("ERROR " + ss.str() + "\n");
        }
    }
    else
    {
        std::stringstream ss;
        ss << status;
        setBody("ERROR " + ss.str() + "\n");
    }
        
    setHeader("Connection", "close");
}
