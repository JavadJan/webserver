/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 00:55:16 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/07 23:04:41 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <unistd.h>
#include <netinet/in.h> // for recv

#include "HTTPRequest.hpp"
#include "Config.hpp"

class Client
{
    private:
        int         _fd;
        std::string _buffer;
        bool        _disconnected;
        
        HTTPRequest _request;
    public:
        Client();
        Client(int fd);
        Client(const Client &copy);
        Client &operator=(const Client &copy);
        ~Client();

        void        setFd(int fd);
        int         getFd()const;
        std::string &getBuffer();
        bool        getConnection()const;
        ssize_t     readData(); 
        ssize_t     sendData(const std::string &data);  
};
