/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 17:29:56 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/18 18:09:28 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "HTTPResponse.hpp"
#include "MIME.hpp"

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>

class HTTPHandler
{
    private:
        ConfigServer        _server;
        HTTPResponse        _res;
        HTTPRequest         _req;
        std::string         _path;
    public:
        HTTPHandler();
        HTTPHandler(const HTTPHandler &copy);
        HTTPHandler &operator=(const HTTPHandler &copy);
        ~HTTPHandler();
            
        void handleRequest();
        void handleGet();
        void handlePost();
        void handleDelete();
        void handleError();
        // void handleCGI();
        // bool isCGI(const std::string &path);
        
        std::string getResponse();
        std::string getDirectotyListing(const std::string &path, const std::string &url);
        
        void setRequest(const HTTPRequest &req);
        void setConfigServer(const ConfigServer &server);
        void setErrorResponse(int status);

        
};
