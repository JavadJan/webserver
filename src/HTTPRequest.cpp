/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:39:11 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/15 19:12:15 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTPRequest.hpp"

HTTPRequest::HTTPRequest(){}

HTTPRequest::HTTPRequest(const HTTPRequest &copy)
{
    *this = copy;
}

HTTPRequest &HTTPRequest::operator=(const HTTPRequest &copy)
{
    if(this != &copy)
    {
           _methode = copy._methode;
           _path = copy._path;
           _version = copy._version;
           _header = copy._header;
           _body = copy._body;  
    }
    
    return *this;
}

HTTPRequest::~HTTPRequest(){}

bool HTTPRequest::isComplete(const std::string &buffer)
{
    // std::cout << "BUFFER IS: " << buffer << std::endl;
    // check if header has end
    size_t headerEnd = buffer.find("\r\n\r\n");
    if(headerEnd == std::string::npos)
    {
        std::cout << "isComplete Error 1" << std::endl;   
        return false;
    }

    size_t methode_pos = buffer.find_first_of(" \t\n\r");
    if(methode_pos == std::string::npos)
        return false;
    std::string methode = buffer.substr(0, methode_pos);
    std::cout << "METHODE :" << methode << std::endl;
    if(methode == "GET" || methode == "DELETE")
    {
        std::cout << "GET or DELETE. ISCOMPLETE" << std::endl;
        return true;
    }
    if(methode != "POST")
    {
        std::cout << "isComplete Error. NO POST" << std::endl;
        return false;
    }
          
    // check Content-Length 
    size_t pos = buffer.find("Content-Length:");
    if(pos == std::string::npos)
    {
        std::cout << "isComplete Error 2" << std::endl;
        return false;
    }

    // get content length value
    
    // skip "Content-Length" and skip space
    pos += 15;
    while(pos < buffer.size() && buffer[pos] == ' ')
        pos++;
    
    size_t endLength = buffer.find("\r\n", pos);
    if(endLength == std::string::npos)
    {
        std::cout << "isComplete Error 3" << std::endl;
        return false;
    }
    
    std::string contentLength_s = buffer.substr(pos, endLength - pos);
    int contentLength = std::atoi(contentLength_s.c_str());
    
    size_t sizeNeeded = headerEnd + 4 + contentLength;
    if(buffer.size() < sizeNeeded)
        return false;

    std::cout << "isComplete DONE" << std::endl;
    return true;
}


// Request Line =   POST /upload HTTP/1.1\r\n
//              =   METHODE     PATH        VERSION
void HTTPRequest::parseRequestLine(const std::string &buffer)
{
    size_t end = buffer.find("\r\n");
    if(end == std::string::npos)
        return;
    
    std::string requestLine = buffer.substr(0, end);
    std::istringstream iss(requestLine);
    std::vector<std::string> words;
    std::string word;

    while(iss >> word)
        words.push_back(word);
    
    if(words.size() == 3)
    {
        _methode    = words[0];
        _path       = words[1];
        _version    = words[2];

        if(_methode.empty() || _path.empty() || _version.empty())
            throw std::runtime_error("Invalid Request Line");
    }
    else
        throw std::runtime_error("Request Line Error");  
}

void HTTPRequest::parseHeaders(const std::string &buffer)
{
    size_t pos = buffer.find("\r\n") + 2;
    while(1)
    {
        size_t end = buffer.find("\r\n", pos);
        if(end == std::string::npos)
            break;
        if(end == pos)
            break;
            
        std::string line = buffer.substr(pos, end-pos);

        size_t separate = line.find(":");
        if(separate == std::string::npos)
            throw std::runtime_error("Key Colon Error");
            
        std::string key = line.substr(0, separate);
        std::string value = line.substr(separate+1);
        
        size_t spaces = 0;
        while(!value.empty() && value[spaces] == ' ')
            spaces++;
        if(spaces > 0)
            value.erase(0, spaces);
            
        _header[key] = value;
        
        pos = end + 2;
    }
}

void HTTPRequest::parseBody(const std::string &buffer)
{
    // std::cout << "parseBody" << std::endl;
    
    if(_header.find("Content-Length") == _header.end())
    {
        std::cout << "DOES NOT EXISTS" << std::endl;
        return;
    }
    else
    {
        size_t contentLength = std::atoi(_header["Content-Length"].c_str());
        size_t bodyStart = buffer.find("\r\n\r\n") + 4;
        // std::cout << "Body Info: " << contentLength << " " << bodyStart << std::endl;
        _body = buffer.substr(bodyStart, contentLength);
        // std::cout << "BODY IS: " << _body << std::endl;
    }
}

void HTTPRequest::parseAll(const std::string &buffer)
{
    // if(isComplete(buffer) == false)
    //     throw std::runtime_error("HTTP Request not complete");
    
    parseRequestLine(buffer);
    parseHeaders(buffer);
    parseBody(buffer);        
}

void HTTPRequest::appendData(const char *buffer, size_t len)
{
    // std::cout << "APPEND: " << buffer << " :: " << len << std::endl;
    _data.append(buffer, len);
}

std::string HTTPRequest::getData()const
{
    return _data;
}

std::string HTTPRequest::getBody()const
{
    return _body;
}

std::string HTTPRequest::getMethode()const
{
    return _methode;
}

std::string HTTPRequest::getPath()const
{
    return _path;
}

std::string HTTPRequest::getVersion()const
{
    return _version;
}

std::map<std::string, std::string> HTTPRequest::getHeader()const
{
    return _header;
}