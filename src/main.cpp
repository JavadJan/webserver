/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 18:34:47 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/06 23:01:47 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Config.hpp"
#include "../includes/HTTPRequest.hpp"

#include <poll.h>
#include <vector>
#include <map>

#define BUFFERSIZE 2048

void insertPollfds(std::vector<struct pollfd> &pfds, int client);

int main()
{
    HTTPRequest req;

    req.parseBody("Hallo");
}

int main1()
{
    Server server(4242);
    // Client client;
    std::vector<struct pollfd> pollfds;
    std::map<int, std::string> clientRequests;
    
    server.createSocket();
    server.setNonBlocking(server.getSocket());
    server.bindSocket();
    server.listenSocket();
    insertPollfds(pollfds, server.getSocket());

    while(true)
    {
        int ret = poll(&pollfds[0], pollfds.size(), 1000);
        if(ret < 0)
        {
            std::cout << "poll. errno: " << errno << std::endl;
            return -1;
        }
        for(size_t i = 0; i < pollfds.size(); i++) 
        {
            if(pollfds[i].revents & POLLIN) 
            {
                if(pollfds[i].fd == server.getSocket())
                {
                    int clientFd = server.acceptClient();
                    if(clientFd >= 0)
                    {
                        // client.setFd(clientFd);
                        server.setNonBlocking(clientFd);
                        insertPollfds(pollfds, clientFd);
                    }
                }
                else
                {
                    // client.readData();
                    char buffer[BUFFERSIZE];
                    ssize_t bytesRead = recv(pollfds[i].fd, buffer, BUFFERSIZE, 0);
                    
                    if(bytesRead > 0)
                    {
                        // handle recieved data

                        clientRequests[pollfds[i].fd].append(buffer, bytesRead);  
                        if(clientRequests[pollfds[i].fd].find("\r\n\r\n"))
                        {
                            std::cout << "FULL HTTP request received:\n" << clientRequests[pollfds[i].fd] << std::endl;
                        }                      
                        // buffer[bytesRead] = '\0';
                        // std::cout << "Recieved:\n" << buffer << std::endl;

                        // std::string response = "Confirmation - Data has been recieved\n";
                        // send(pollfds[i].fd, response.c_str(), response.size(), 0);
                    }
                    else if(bytesRead == 0)
                    {
                        // client closed the connection
                        std::cout << "Client has closed the connection" << std::endl;
                        close(pollfds[i].fd); 
                        pollfds.erase(pollfds.begin() + i);
                        i--;
                    }
                    else
                    {
                        // non-blocking read error. 
                        // ignore
                    }
                }
            }
        }
    }

    return 0;
}

void insertPollfds(std::vector<struct pollfd> &pfds, int client)
{
    struct pollfd pfd;
    pfd.fd = client;
    pfd.events = POLLIN;
    pfd.revents = 0;
    pfds.push_back(pfd);
}


// --------------------------------------------

// TEST 2

// int main()
// {
//     Server server(4242);
//     server.createSocket();
//     // server.setNonBlocking(server.getSocket());
//     server.bindSocket();
//     server.listenSocket();

//     while(true)
//     {
//         int client = server.acceptClient();
//         if(client >= 0)
//         {
//             char buffer[10];
//             size_t bytesRead = read(client, buffer, 10);
//             if(bytesRead > 0)
//             {
//                 buffer[bytesRead] = '\0';
//                 std::cout << "Recieved:\n" << buffer << std::endl;

//                 std::string response = "COOL\n";
//                 send(client, response.c_str(), response.size(), 0);
//             }
//             close(client);     
//         }
//         else
//             break;
//     }
    
//     return 0;
// }


// --------------------------------------------
// TEST 1

// #include <sys/socket.h>
// #include <iostream>
// #include <netinet/in.h> //for sockaddr_in
// #include <cstdlib>
// #include <unistd.h>
// #include <cerrno>

// Terminal for client: telnet -4 localhost 4242

// int main()
// {   
    
//     int _socket;
//     int _bind;
//     int _listen;
//     int _connection;
    
//     std::cout << "socket" << std::endl;
//     _socket = socket(AF_INET, SOCK_STREAM, 0);
//     if(_socket < 0)
//     {
//         std::cout << "Failed to create socket. errno: " << errno << std::endl;
//         return -1;
//     }
        
//     struct sockaddr_in sockaddr;
//     sockaddr.sin_family = AF_INET;
//     sockaddr.sin_addr.s_addr = INADDR_ANY;
//     sockaddr.sin_port = htons(PORT);
    
//     std::cout << "bind" << std::endl;
//     _bind = bind(_socket, (struct sockaddr*)&sockaddr, sizeof(sockaddr));
//     if(_bind < 0)
//     {
//         std::cout << "Failed to bind. errno: " << errno << std::endl;
//         return -1;
//     }
//     std::cout << "listen" << std::endl;
//     _listen = listen(_socket, 10);
//     if(_listen < 0)
//     {
//         std::cout << "Failed to listen. errno: " << errno << std::endl;
//         return -1;
//     }
    
//     socklen_t addrlen = sizeof(sockaddr);
//     std::cout << "accept" << std::endl;
//     _connection = accept(_socket, (struct sockaddr*)&sockaddr, &addrlen);
//     if(_connection < 0)
//     {
//         std::cout << "Failed to accept. errno: " << errno << std::endl;
//         return -1;
//     }
        
//     std::cout << "read" << std::endl;
//     char buffer[100];
//     size_t bytesRead = read(_connection, buffer, 100);
//     std::cout << "The message was: " << buffer << std::endl;

//     std::cout << "send" << std::endl;
//     std::string response = "GOOD TALKING\n";
//     send(_connection, response.c_str(), response.size(), 0);
    
//     std::cout << "close" << std::endl;
//     close(_connection);
//     close(_socket);
    
//     (void)bytesRead;
//     return 0;
// }
