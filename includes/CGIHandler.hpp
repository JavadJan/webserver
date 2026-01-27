/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGIHandler.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/18 17:23:24 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/19 01:22:44 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"
#include "HTTPResponse.hpp"
#include <iostream>
#include <map>
#include <sys/wait.h>

class CGIHandler
{
    private:
        int _toCGI[2];
        int _fromCGI[2];
        std::map<std::string, std::string> _env;
        char **_c_env;
        HTTPResponse _res;
        HTTPRequest  _req;
        ConfigServer _server;
        
    public:
        CGIHandler();
        CGIHandler(const CGIHandler &copy);
        CGIHandler &operator=(const CGIHandler &copy);
        ~CGIHandler();
        
        void setRequest(const HTTPRequest &req);
        void setConfigServer(const ConfigServer &server);
        void setErrorResponse(int status);
        
        std::string getResponse();
        
        void buildEnvironment();
        void handleCGI();
        bool isCGI(const std::string &path);
};
