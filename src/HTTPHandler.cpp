/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 17:36:47 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/09 15:55:06 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTPHandler.hpp"
#include "../includes/MIME.hpp"

HTTPHandler::HTTPHandler()
{
    
}

HTTPHandler::HTTPHandler(const HTTPHandler &copy)
{
    *this = copy;
}

HTTPHandler &HTTPHandler::operator=(const HTTPHandler &copy)
{
    if(this != &copy)
    { 
    }
    
    return *this;
}

HTTPHandler::~HTTPHandler(){}

void HTTPHandler::handleRequest(const HTTPRequest &req)
{
    _path = req.getPath();
    if(_path == "/")
        _path = "/index.html";
        
    MIME mime;
    std::string sfx = mime.getSuffix(_path);
    _res.setHeader("Content-Type", mime.getMimeType(sfx));
    
    if(req.getMethode() == "GET")
        handleGet();
    else if(req.getMethode() == "POST")
        handlePost();
    else if(req.getMethode() == "DELETE")
        handleDelete();
    else
        handleError();
}


void HTTPHandler::handleGet()
{
    std::string fullPath = "./www" + _path;
    
    std::cout << "FULL PATH: " << fullPath << std::endl;
    std::cout << "FULL BODY: " << _req.getBody() << std::endl;
    
    struct stat st;
    // check if exist or can be accessed
    if(stat(fullPath.c_str(), &st) < 0)
    {
        // TO DO - write RESPONSE - NO FILE
        _res.setStatus(600, _res.reasonPhrase(600));
        _res.setBody("ERROR - FILE OR DIRECTORY NOT EXISTS\n");
        return;
        // throw std::runtime_error("ERROR - FILE OR DIRECTORY NOT EXISTS");
    }
    
    // S_ISDIR is a macro to help determine if the path is a directory after calling stat()
    if(S_ISDIR(st.st_mode))
    {
        std::string indexPath = fullPath + "/index.html";
        if(stat(indexPath.c_str(), &st) == 0 && S_ISREG(st.st_mode))
        {
            fullPath = indexPath;
            // MIME mime;
            // std::string sfx = mime.getSuffix(fullPath);
            // _res.setHeader("Content-Type", mime.getMimeType(sfx));  
        }
        else
        {
            _res.setStatus(600, _res.reasonPhrase(600));
            _res.setBody("ERROR - IS NOT A DIRECTORY\n");
            return;
        }
            
        // throw std::runtime_error("ERROR - IS NOT A DIRECTORY");
            // TO DO - write RESPONSE - NO PERMISSION / FORBIDDEN
    }

    int fd = open(fullPath.c_str(), O_RDONLY);
    if(fd < 0)
    {
        // TO DO - write OPEN ERROR 
        _res.setStatus(600, _res.reasonPhrase(600));
        _res.setBody("ERROR - OPEN FILE\n");
        return;
        // throw std::runtime_error("ERROR - OPEN FILE");
    }

    std::string body;
    char buffer[BUFFERSIZE];
    ssize_t bytes;
    while(1)
    {
        bytes = read(fd, buffer, sizeof(buffer));
        std::cout << "BUFFER: " << buffer << std::endl;
        if(bytes <= 0)
            break;
        body.append(buffer, bytes);
    }
    close(fd);

    _res.setStatus(200, _res.reasonPhrase(200));
    _res.setBody(body);
    _res.setHeader("Connection", "close");
    std::string response = _res.response();
}

void HTTPHandler::handlePost()
{
    if(_req.getHeader().find("Content-Length") == _req.getHeader().end())
    {
        _res.setStatus(411, _res.reasonPhrase(411));
        _res.setBody("ERROR POST - NO CONTENT LENGTH\n");
        return;
        // throw std::runtime_error("POST - NO CONTENT LENGTH");
    }
    
    std::cout << "FULL BODY: " << _req.getBody() << std::endl;
    if(_req.getBody().empty())
    {
        _res.setStatus(400, _res.reasonPhrase(400));
        _res.setBody("ERROR POST - NO BODY\n");
        return;
        // throw std::runtime_error("POST - NO BODY");
    }

    if(_path != "/uploads")
    {
        _res.setStatus(404, _res.reasonPhrase(400));
        _res.setBody("ERROR POST - NOT FOUND\n");
        return;
        // throw std::runtime_error("POST - NOT FOUND");
    }
    
    std::string uploadPath = "./www/uploads" + _path;
        
    int fd = open(uploadPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0)
    {
        _res.setStatus(500, _res.reasonPhrase(500));
        _res.setBody("ERROR POST - OPEN ERROR\n");
        return;
        // throw std::runtime_error("POST - OPEN ERROR");
    }

    ssize_t written = write(fd, _req.getBody().c_str(), _req.getBody().size());
    if(written < 0)
    {
        _res.setStatus(500, _res.reasonPhrase(500));
        _res.setBody("ERROR POST - WRITE ERROR\n");
        return;
        // throw std::runtime_error("POST - OPEN ERROR");
    }
    
    _res.setStatus(200, _res.reasonPhrase(200));
    _res.setBody("File uploaded\n");
    _res.setHeader("Connection", "close");
    std::string response = _res.response();
}

void HTTPHandler::handleDelete()
{
    std::string filePath = "./www" + _path;
    
    struct stat st;
    // check if exists
    if(stat(filePath.c_str(), &st) < 0)
    {
        _res.setStatus(500, _res.reasonPhrase(500));
        _res.setBody("ERROR DELETE - PATH DOES NOT EXIST\n");
        return;
        // throw std::runtime_error("DELETE - PATH DOES NOT EXIST");
    }

    //check if its a file
    if(S_ISREG(st.st_mode))
    {
        int unl = unlink(filePath.c_str());
        if(unl == 0)
        {
            _res.setStatus(200, _res.reasonPhrase(200));
            _res.setBody("File deletet\n");
        }
        else
        {
            _res.setStatus(403, _res.reasonPhrase(403));
            _res.setBody("ERROR DELETE - File Could Not be deleted\n");
            return;
            // throw std::runtime_error("DELETE - File Could Not be deleted");
        }
    }
    else
    {
        _res.setStatus(404, _res.reasonPhrase(404));
        _res.setBody("ERROR DELETE - PATH IS NOT A FILE\n");
        return;
        // throw std::runtime_error("DELETE - PATH IS NOT A FILE");
    }
}

void HTTPHandler::handleError()
{
    _res.setStatus(500, _res.reasonPhrase(500));
    _res.setBody("METHODE NOT ALLOWED\n");
    // throw std::runtime_error("METHODE NOT ALLOWED");
}

std::string HTTPHandler::getResponse()
{
    return _res.response();
}

void HTTPHandler::setRequest(const HTTPRequest &req)
{
    _req = req;
}
