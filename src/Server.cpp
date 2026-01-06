/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 11:34:46 by asemykin          #+#    #+#             */
/*   Updated: 2025/12/22 01:07:43 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"

Server::Server()
{
    _port = PORT;
}

Server::Server(int port)
{
    _port = port;
}

Server::Server(const Server &copy)
{
    *this = copy;
}

Server &Server::operator=(const Server &copy)
{
    if(this != &copy)
    {
        _port = copy._port;
        _socket = copy._socket;
    }

    return *this;
}

Server::~Server()
{}

int Server::getSocket()
{
    return _socket;
}

int Server::getPort()
{
    return _port;
}


// AF_INET         = IPv4
// SOCK_STREAM     = TCP (required for HTTP)
// 0               = default protocol
int Server::createSocket()
{
    _socket = socket(AF_INET, SOCK_STREAM, 0);
    if(_socket < 0)
    {
        std::cout << "Socket Error. errno: " << std::endl;
        return -1;
    }
    return 0;
}

int Server::bindSocket()
{
    int                 status;
    struct sockaddr_in  sockaddr;
    
    sockaddr.sin_family = AF_INET;
    sockaddr.sin_addr.s_addr = INADDR_ANY;
    sockaddr.sin_port = htons(_port);
    
    status = bind(_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
    if(status < 0)
    {
        std::cout << "Bind Error. errno: " << errno << std::endl;
        return -1;
    }
    
    return 0;
}

int Server::listenSocket()
{
    int status;

    status = listen(_socket, 10);
    if(status < 0)
    {
        std::cout << "Listen Error. errno: " << errno << std::endl;
        return -1;
    }

    return 0;
}

int Server::acceptClient()
{
    int clientFd;
    // struct sockaddr_in sadd;
    // socklen_t slen = sizeof(sadd);
    
    clientFd = accept(_socket, NULL, NULL);
    if(clientFd < 0)
    {
        std::cout << "Accept Error. errno: " << errno << std::endl;
        return -1;
    }

    return clientFd;
}

void Server::setNonBlocking(int fd)
{
    int status;

    status = fcntl(fd, F_SETFL, O_NONBLOCK);
    if(status < 0)
        std::cout << "fcntl Error. errno: " << errno << std::endl;
        
}
