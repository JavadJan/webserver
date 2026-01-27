/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:57:39 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/19 01:31:30 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/CGIHandler.hpp"

CGIHandler::CGIHandler()
{
    
}

CGIHandler::CGIHandler(const CGIHandler &copy)
{
    *this = copy;
}

CGIHandler &CGIHandler::operator=(const CGIHandler &copy)
{
    if(this != &copy)
    { 
        _server = copy._server;
        _res = copy._res;
        _env = copy._env;
        _c_env = copy._c_env;
        _toCGI[0] = copy._toCGI[0];
        _toCGI[1] = copy._toCGI[1];
        _fromCGI[0] = copy._fromCGI[0];
        _fromCGI[1] = copy._fromCGI[1];
    }
    
    return *this;
}

CGIHandler::~CGIHandler(){}

void CGIHandler::setRequest(const HTTPRequest &req)
{
    _req = req;
}

void CGIHandler::setErrorResponse(int status)
{
    _res.setErrorResponse(status, _server.errorPages[status]);
}

std::string CGIHandler::getResponse()
{
    return _res.response();
}
#include <unistd.h>
#include <string.h>
void CGIHandler::buildEnvironment()
{
    // std::map<std::string, std::string> _env;

    _env["REQUEST_METHOD"] = "hallo";//_req.getMethode();
    // _env["QUERY_STRING"] = ;
    // _env["CONTENT_LENGTH"] = ;
    // _env["CONTENT_TYPE"] = ;
    // _env["SCRIPT_NAME"] = ;
    // _env["SERVER_NAME"] = ;
    // _env["SERVER_PORT"] = ;
    // _env["SERVER_PROTOCOL"] = ;
    // _env["REMOTE_ADDR"] = ;
    // _env["PATH_INFO"] = ;
    // _env["PATH_TRANSLATED"] = ;
    // _env["GATEWAY_INTERFACE"] = ;
    _c_env = (char **)malloc(sizeof(char *) * (_env.size() + 1));
    if(_c_env == NULL)
    {
        setErrorResponse(500);
        return;
    }
    
    int i = 0;
    for(std::map<std::string, std::string>::iterator it = _env.begin(); 
        it != _env.end(); it++)
    {
        std::string envTmp = it->first + "=" + it->second;
        _c_env[i] = strdup(envTmp.c_str());
        std::cout << _c_env[i] << std::endl;
        i++;
    }
    _c_env[i] = NULL;
    
}

void CGIHandler::setConfigServer(const ConfigServer &server)
{
    _server = server;
}

bool CGIHandler::isCGI(const std::string &path)
{
    std::string sfx = MIME().getSuffix(path);
    if(sfx == ".php" || sfx == ".py")
        return true;
    
    return false;
}



void CGIHandler::handleCGI()
{       
    const ConfigLocations *loca = _server.getMatchingLocation(_req.getPath());

    std::cout << "CGI 00" << std::endl;
    
    if(!loca)
    {
        setErrorResponse(600);
        return;
    }
    
    std::cout << "CGI 0" << std::endl;
    if(!loca->isMethodAllowed(_req.getMethode()))
    {
        setErrorResponse(600);
        return;
    }

    std::cout << "CGI 1" << std::endl;
    std::string path = "./www" + _req.getPath();
    
    if(pipe(_toCGI) < 0 || pipe(_fromCGI) < 0)
    {
        setErrorResponse(500);
        return;
    }

    std::cout << "CGI 2" << std::endl;
    pid_t pid = fork();
    if(pid < 0)
    {
        setErrorResponse(500);
        return;
    }
    std::cout << "CGI 3" << std::endl;
    if(pid == 0)
    {
        dup2(_toCGI[0], STDIN_FILENO);
        dup2(_fromCGI[1], STDOUT_FILENO);
        close(_toCGI[1]);
        close(_fromCGI[0]);

        buildEnvironment();

        char *argv[] = {(char*)path.c_str(), NULL};

        int exit_status = 0;
        exit_status = execve(path.c_str(), argv, _c_env);

        exit(exit_status);        
    }
    std::cout << "CGI 4" << std::endl;
    close(_toCGI[0]);
    close(_fromCGI[1]);

    std::string body = _req.getBody();
    // CGI reads the request from the stdin.
    write(_toCGI[1], body.c_str(), body.size());
    close(_toCGI[1]);

    std::cout << "CGI sending :" << body.c_str() << std::endl;
    // now read CGI output
    char buffer[BUFFERSIZE];
    std::string cgiOut;
    while(1)
    {
        ssize_t n = read(_fromCGI[0], buffer, sizeof(buffer));
        if(n <= 0)
            break;
        cgiOut.append(buffer, n);
    }
    close(_fromCGI[0]);
    
    int status;
    waitpid(pid, &status, 0);

    std::cout << "CGI 5" << std::endl;
    if(WEXITSTATUS(status) != 0)
    {
        setErrorResponse(500);
        return;
    }

    std::cout << "CGI 6" << std::endl;
    _res.setStatus(200, _res.reasonPhrase(200));
    _res.setBody(cgiOut);
    _res.setHeader("Content-Type", MIME().getMimeType(".html"));
}
