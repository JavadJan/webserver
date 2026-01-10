/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/23 01:05:29 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/10 00:58:59 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Client.hpp"

Client::Client():_fd(-1), _buffer(""), _disconnected(false)
{}

Client::Client(int fd):_fd(fd), _buffer(""), _disconnected(false)
{}
Client::Client(const Client &copy)
{
    *this = copy;
}

Client &Client::operator=(const Client &copy)
{
    if(this != &copy)
    {
        _fd = copy._fd;
        _buffer = copy._buffer;
        _disconnected = copy._disconnected;
    }

    return *this;
}

Client::~Client()
{
    if(_fd >= 0)
        close(_fd);
}

void Client::setFd(int fd)
{
    _fd = fd;
}

int Client::getFd()const
{
    return _fd;
}

std::string &Client::getBuffer()
{
    return _buffer;
}

bool Client::getConnection()const
{
    return _disconnected;
}

ssize_t Client::readData()
{
    char buffer[BUFFERSIZE];
    ssize_t bytesRead = recv(_fd, buffer, sizeof(buffer), 0);
    
    if(bytesRead > 0)
    {
        // append because requests can come in more chunks.
        // so we need to kepp earlier requests.
        // otherwise we could lose earlier parts
        _buffer.append(buffer, bytesRead);
        return bytesRead;
    }
    else if(bytesRead == 0)
    {
        _disconnected = true;
        return 0;
    }
    else
    {
        _disconnected = true;
        return -1;
    }
}

ssize_t Client::sendData(const std::string &data)
{
    ssize_t bytesSend = send(_fd, data.c_str(), data.size(), 0);
    if(bytesSend < 0)
    {
        // errno for return -1 ?
        return 0;
    }    
    return bytesSend;
}
