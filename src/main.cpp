/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 18:34:47 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/10 00:58:23 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Config.hpp"
#include "../includes/HTTPRequest.hpp"
#include "../includes/HTTPResponse.hpp"
#include "../includes/HTTPHandler.hpp"

#include <poll.h>
#include <vector>
#include <map>



void insertPollfds(std::vector<struct pollfd> &pfds, int client);


#include <string>
int main(int argv, char **argc)
{
    if(argv != 2)
        return 1;
    // Server server(4243);
    Server server(std::atoi(argc[1]));
    // Client client;
    std::vector<struct pollfd> pollfds;
    std::map<int, HTTPRequest> clientRequests;
    
    server.createSocket();
    server.setNonBlocking(server.getSocket());
    server.bindSocket();
    server.listenSocket();
    insertPollfds(pollfds, server.getSocket());

    while(true)
    {
        int ret = poll(&pollfds[0], pollfds.size(), 1000);
        // std::cout << "1" << std::endl;
        if(ret < 0)
        {
            std::cout << "poll. errno: " << errno << std::endl;
            return -1;
        }
        for(size_t i = 0; i < pollfds.size(); i++) 
        {
            if(pollfds[i].revents & POLLIN) 
            {
                std::cout << "2" << std::endl;
                if(pollfds[i].fd == server.getSocket())
                {
                    std::cout << "3" << std::endl;
                    int clientFd = server.acceptClient();
                    if(clientFd >= 0)
                    {
                        std::cout << "4" << std::endl;
                        // client.setFd(clientFd);
                        server.setNonBlocking(clientFd);
                        insertPollfds(pollfds, clientFd);
                        clientRequests[clientFd] = HTTPRequest();
                    }
                }
                else
                {
                    std::cout << "5" << std::endl;
                    // client.readData();
                    char buffer[BUFFERSIZE];
                    ssize_t bytesRead = recv(pollfds[i].fd, buffer, BUFFERSIZE, 0);
                    
                    if(bytesRead > 0)
                    {
                        std::cout << "6" << std::endl;
                        // handle recieved data

                        clientRequests[pollfds[i].fd].appendData(buffer, bytesRead);
                          
                        if(clientRequests[pollfds[i].fd].isComplete(clientRequests[pollfds[i].fd].getData()))
                        {
                            clientRequests[pollfds[i].fd].parseAll(clientRequests[pollfds[i].fd].getData());
                            std::cout << "Methode: " << clientRequests[pollfds[i].fd].getMethode();
                            std::cout << "\nPath: " << clientRequests[pollfds[i].fd].getPath();
                            std::cout << "\nVersion: " << clientRequests[pollfds[i].fd].getVersion();
                            std::cout << "\nBody: " << clientRequests[pollfds[i].fd].getBody() << "\n\n";
                            
                            // HTTPResponse httpRes;
                            // httpRes.setStatus(200, "OK");
                            // httpRes.setHeader("Content-Type", "text/plain");
                            // std::string body = "Hello world\n";
                            // std::stringstream ss;
                            // ss << body.size();
                            // httpRes.setHeader("Content-Length", ss.str());
                            // httpRes.setBody(body);
                            
                            // HTTPResponse httpRes;
                            // httpRes.setStatus(200, "OK");
                            // httpRes.setBody("Hello world\n");
                            // std::string response = httpRes.response();

                            // send(pollfds[i].fd, response.c_str(), response.size(), 0);
                            
                            HTTPHandler han;
                            han.setRequest(clientRequests[pollfds[i].fd]);
                            han.handleRequest(clientRequests[pollfds[i].fd]);
                            // httpRes.setStatus(200, "OK");
                            // httpRes.setBody("Hello world\n");
                            std::string response = han.getResponse();

                            send(pollfds[i].fd, response.c_str(), response.size(), 0);
                            
                            close(pollfds[i].fd);
                            pollfds.erase(pollfds.begin() + i);
                            clientRequests.erase(pollfds[i].fd);
                        }      
                        else
                            std::cout << "7" << std::endl;                
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
