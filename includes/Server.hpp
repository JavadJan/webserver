/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 11:23:25 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/07 23:05:03 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "Config.hpp"

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h> //for sockaddr_in
#include <cstdlib>
#include <unistd.h>
#include <cerrno>
#include <fcntl.h>
// #include <stdexcept>

# define PORT 4242

class Server
{
    private:
        int _port;
        int _socket;
        // struct addrinfo *_res;
    public:
        Server();
        Server(int port);
        Server(const Server &copy);
        Server &operator=(const Server &copy);
        ~Server();

        int createSocket();
        int bindSocket();
        int listenSocket();
        int acceptClient();
        void setNonBlocking(int fd);
        
        int getPort();
        int getSocket();
};
