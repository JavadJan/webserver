/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPResponse.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/06 23:22:38 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/09 14:46:47 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "HTTPRequest.hpp"
#include "Config.hpp"

#include <iostream>
#include <map>
#include <sstream>

class HTTPResponse
{
    private:
        std::string _version;
        int _status;
        std::string _reason;
        std::map<std::string, std::string> _headers;
        std::string _body;
    public:
        HTTPResponse();
        HTTPResponse(const HTTPResponse &copy);
        HTTPResponse &operator=(const HTTPResponse &copy);
        ~HTTPResponse();

        void setStatus(int status, const std::string &reason);
        void setHeader(const std::string &key, const std::string &value);
        void setBody(const std::string &body);
        std::string getHeaderType(const std::string &key);
        std::string reasonPhrase(int status);
        std::string response();

        // void handleResponse(const HTTPRequest &req);
};
