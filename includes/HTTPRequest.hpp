/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:36:21 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/09 01:16:57 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"

#include <iostream>
#include <map>
#include <vector>
#include <sstream>
#include <cstdlib>

class HTTPRequest
{
    private:
        std::string _methode;
        std::string _path;
        std::string _version;
        std::map<std::string, std::string> _header;
        std::string _body;

        std::string _data;
                
    public:
        HTTPRequest();
        HTTPRequest(const HTTPRequest &copy);
        HTTPRequest &operator=(const HTTPRequest &copy);
        ~HTTPRequest();
        
        bool isComplete(const std::string &buffer);
        void parseRequestLine(const std::string &buffer);
        void parseHeaders(const std::string &buffer);
        void parseBody(const std::string &buffer);
        void parseAll(const std::string &buffer);

        void appendData(const char *buffer, size_t len);

        // get
        std::string getMethode()const;
        std::string getPath()const;
        std::string getVersion()const;
        std::map<std::string, std::string> getHeader()const;
        std::string getData()const;
        std::string getBody()const;
        
};
