/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPHandler.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/07 17:29:56 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/08 23:45:19 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HTTPResponse.hpp"
#include "Config.hpp"

#include <iostream>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

class HTTPHandler
{
    private:
        HTTPResponse    _res;
        HTTPRequest     _req;
        std::string     _path;
    public:
        HTTPHandler();
        HTTPHandler(const HTTPHandler &copy);
        HTTPHandler &operator=(const HTTPHandler &copy);
        ~HTTPHandler();
            
        void setRequest(const HTTPRequest &req);
        void handleRequest(const HTTPRequest &req);
        void handleGet();
        void handlePost();
        void handleDelete();
        void handleError();
        std::string getResponse();
};
