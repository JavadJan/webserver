/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPRequest.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/02 20:36:21 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/07 00:16:26 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
};
