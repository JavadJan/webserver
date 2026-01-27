/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHandler.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 17:36:47 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/18 18:33:48 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/HTTPHandler.hpp"


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
        _server = copy._server;
        _res = copy._res;
        _req = copy._req;
        _path = copy._path;
    }
    
    return *this;
}

HTTPHandler::~HTTPHandler(){}

void HTTPHandler::setConfigServer(const ConfigServer &server)
{
    _server = server;
}

        
void HTTPHandler::handleRequest()
{
    _path = _req.getPath();
    // if(_path == "/")
    //     _path = "/index.html";

    const ConfigLocations *loca = _server.getMatchingLocation(_path);
    std::cout << "AAA 1" << std::endl;
    if(!loca)
    {
        setErrorResponse(600);
        return;
    }
    std::cout << "AAA 2" << std::endl;
    // std::cout << "MATCH was :" << match->path << std::endl;
    
    // std::cout << "ENTER 1:" << loca->index << ":" <<  loca->index << ":" << std::endl;
    if(!loca->isMethodAllowed(_req.getMethode()))
    {
        setErrorResponse(600);
        return;
    }
    std::cout << "AAA 3" << std::endl;
    // std::cout << "ENTER 2:" << loca->index << ":" <<  std::endl;
    // MIME mime;
    // std::string sfx = mime.getSuffix(_path);
    // _res.setHeader("Content-Type", mime.getMimeType(sfx));
    
    // if(isCGI(_path))
    //     handleCGI();
    if(_req.getMethode() == "GET")
        handleGet();
    else if(_req.getMethode() == "POST")
        handlePost();
    else if(_req.getMethode() == "DELETE")
        handleDelete();
    else
        handleError();
}

void HTTPHandler::handleGet()
{
    std::string fullPath = "./www" + _path;
    
    std::cout << "FULL PATH: " << fullPath << std::endl;
    std::cout << "FULL BODY: " << _req.getBody() << std::endl;
    
    std::cout << "HERE 0" << std::endl;
    struct stat st;
    // check if exist or can be accessed
    if(stat(fullPath.c_str(), &st) < 0)
    {
        // TO DO - write RESPONSE - NO FILE
        setErrorResponse(600);
        return;
        // throw std::runtime_error("ERROR - FILE OR DIRECTORY NOT EXISTS");
    }
    std::cout << "HERE 000" << std::endl;
    // S_ISDIR is a macro to help determine if the path is a directory after calling stat()
    if(S_ISDIR(st.st_mode))
    {
        std::cout << "HERE 1" << std::endl;
        const ConfigLocations *loca = _server.getMatchingLocation(_path);
        
        std::string indexPath = fullPath + "/" + loca->index; //"index.html";
        
        if(!loca->index.empty() && 
            stat(indexPath.c_str(), &st) == 0 && 
            S_ISREG(st.st_mode))
        {
            std::cout << "HERE 2" << std::endl;
            fullPath = indexPath;
            // MIME mime;
            // std::string sfx = mime.getSuffix(fullPath);
            // _res.setHeader("Content-Type", mime.getMimeType(sfx));  
        }
        else
        {
            std::cout << "HERE 3" << std::endl;
            if(!loca->autoindex)
            {
                std::cout << "HERE 4" << std::endl;
                setErrorResponse(600);
                return;
            }
            std::cout << "HERE 5" << std::endl;
            std::string body = getDirectotyListing(fullPath, _path);
            _res.setStatus(200, _res.reasonPhrase(200));
            _res.setHeader("Content-Type", MIME().getMimeType(".html"));
            _res.setBody(body);
            return;
        }
    }

    int fd = open(fullPath.c_str(), O_RDONLY);
    if(fd < 0)
    {
        // TO DO - write OPEN ERROR 
        setErrorResponse(600);
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

    // setErrorResponse(200);
    _res.setStatus(200, _res.reasonPhrase(200));
    _res.setBody(body);
    _res.setHeader("Connection", "close");
    std::string response = _res.response();
}

void HTTPHandler::handlePost()
{
    if(_req.getHeader().find("Content-Length") == _req.getHeader().end())
    {
        setErrorResponse(411);
        // _res.setStatus(411, _res.reasonPhrase(411));
        // _res.setBody("ERROR POST - NO CONTENT LENGTH\n");
        return;
        // throw std::runtime_error("POST - NO CONTENT LENGTH");
    }
    
    std::cout << "FULL BODY: " << _req.getBody() << std::endl;
    if(_req.getBody().empty())
    {
        setErrorResponse(400);
        // _res.setStatus(400, _res.reasonPhrase(400));
        // _res.setBody("ERROR POST - NO BODY\n");
        return;
        // throw std::runtime_error("POST - NO BODY");
    }

    std::cout << "DIR :" << _path << ":" << std::endl;
    if(_path != "/uploads")
    {
        setErrorResponse(404);
        // _res.setStatus(404, _res.reasonPhrase(400));
        // _res.setBody("ERROR POST - NOT FOUND\n");
        return;
        // throw std::runtime_error("POST - NOT FOUND");
    }
    
    std::string uploadPath = "./www/uploads" + _path;
        
    int fd = open(uploadPath.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if(fd < 0)
    {
        setErrorResponse(500);
        // _res.setStatus(500, _res.reasonPhrase(500));
        // _res.setBody("ERROR POST - OPEN ERROR\n");
        return;
        // throw std::runtime_error("POST - OPEN ERROR");
    }

    ssize_t written = write(fd, _req.getBody().c_str(), _req.getBody().size());
    if(written < 0)
    {
        setErrorResponse(500);
        // _res.setStatus(500, _res.reasonPhrase(500));
        // _res.setBody("ERROR POST - WRITE ERROR\n");
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
        setErrorResponse(500);
        // _res.setStatus(500, _res.reasonPhrase(500));
        // _res.setBody("ERROR DELETE - PATH DOES NOT EXIST\n");
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
            setErrorResponse(403);
            // _res.setStatus(403, _res.reasonPhrase(403));
            // _res.setBody("ERROR DELETE - File Could Not be deleted\n");
            return;
            // throw std::runtime_error("DELETE - File Could Not be deleted");
        }
    }
    else
    {
        setErrorResponse(404);
        // _res.setStatus(404, _res.reasonPhrase(404));
        // _res.setBody("ERROR DELETE - PATH IS NOT A FILE\n");
        return;
        // throw std::runtime_error("DELETE - PATH IS NOT A FILE");
    }
}

void HTTPHandler::handleError()
{
    setErrorResponse(500);
    // _res.setStatus(500, _res.reasonPhrase(500));
    // _res.setBody("METHODE NOT ALLOWED\n");
    // throw std::runtime_error("METHODE NOT ALLOWED");
}

std::string HTTPHandler::getDirectotyListing(const std::string &path, const std::string &url)
{
    (void)url;
    
    DIR *dir = opendir(path.c_str());
    if(!dir)
    {
        // std::cout << "NOT DIR" << std::endl;
        return "";
    }
    // std::cout << "IN DIR" << std::endl;

    std::stringstream html;

    // html << "<html>\n";
    // html << "\t<head>\n";
    // html << "\t\t<titel>Index of " << url << "</titel>\n";
    // html << "\t</head>\n";
    // html << "\t<body>\n";
    // html << "\t\t<h1>Index of " << url << "</h1>\n";
    // html << "<hr><ul>";
    
    html << "<html>";
    html << "<head><titel>" << url << "</titel></head>";
    html << "<body>";
    html << "<h1>" << url << "</h1>";
    html << "<hr><ul>";
    
    struct dirent *files;
    while(1)
    {
        files = readdir(dir);
        if(files == NULL)
            break;
        std::string d_name = files->d_name;
        if(d_name == "." || d_name == "..")
            continue;
        // std::cout << d_name << std::endl;
        html << "<li><a href=\"" << url;
        if(url[url.size()-1] != '/')
            html << "/";
        html << d_name << "\">" << d_name << "</a></li>";
    }
    html << "</ul><hr></body></html";

    closedir(dir);
    
    std::cout << html.str() << std::endl;
    return html.str();    
}

std::string HTTPHandler::getResponse()
{
    return _res.response();
}

void HTTPHandler::setRequest(const HTTPRequest &req)
{
    _req = req;
}

void HTTPHandler::setErrorResponse(int status)
{
    _res.setErrorResponse(status, _server.errorPages[status]);
    
    // _res.setStatus(status, _res.reasonPhrase(status));
    
    // std::string path = _server.errorPages[status];
    // if(!path.empty())
    // {
    //     int fd = open(path.c_str(), O_RDONLY);
    //     if(fd >= 0)
    //     {
    //         std::string body;
    //         char buffer[BUFFERSIZE];
    //         ssize_t bytes;
    //         while(1)
    //         {
    //             bytes = read(fd, buffer, sizeof(buffer));
    //             if(bytes <= 0)
    //                 break;
    //             body.append(buffer, bytes);
    //         }
    //         close(fd);

    //         _res.setBody(body);
            
    //         std::string sfx = MIME().getSuffix(path);
    //         _res.setHeader("Content-Type", MIME().getMimeType(sfx));
    //     }
    //     else
    //     {
    //         std::stringstream ss;
    //         ss << status;
    //         _res.setBody("ERROR " + ss.str() + "\n");
    //     }
    // }
    // else
    // {
    //     std::stringstream ss;
    //     ss << status;
    //     _res.setBody("ERROR " + ss.str() + "\n");
    // }
        
    // _res.setHeader("Connection", "close");
}

// bool HTTPHandler::isCGI(const std::string &path)
// {
//     std::string sfx = MIME().getSuffix(path);
//     if(sfx == ".php" || sfx == ".py")
//         return true;
    
//     return false;
// }

// void HTTPHandler::handleCGI()
// {    
//     // int 
//     // std::string path = "./www" + _path;
// }
