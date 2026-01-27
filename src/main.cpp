/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: asemykin <asemykin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/21 18:34:47 by asemykin          #+#    #+#             */
/*   Updated: 2026/01/19 01:41:04 by asemykin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/Server.hpp"
#include "../includes/Client.hpp"
#include "../includes/Config.hpp"
#include "../includes/HTTPRequest.hpp"
#include "../includes/HTTPResponse.hpp"
#include "../includes/HTTPHandler.hpp"
#include "../includes/CGIHandler.hpp"

#include <poll.h>
#include <vector>
#include <map>



void insertPollfds(std::vector<struct pollfd> &pfds, int client);

/*
echo -e "DELETE /dTest.txt HTTP/1.1\r\nHost: loc
alhost\r\nContent-Length: 11\r\n\r\nHello World" | nc localhost 4242

echo -e "GET /uploads/test.php HTTP/1.
1\r\nHost: localhost\r\n\r\n" | nc localhost 4242

*/

#include <sys/wait.h>
void forkExample()
{
    int fd[2]; //fd[0] STDIN, fd[1] STDOUT
    pipe(fd);
    
    pid_t pid = fork();
    
    // in child process
    if(pid == 0)
    {
        int x = 10;
        close(fd[0]);
        for (int i = 0; i < x;i++)
        {
            std::string msg = "HELLO WORLD!!";
            write(fd[1], msg.c_str(), msg.size());
            usleep(1);
        }
        close(fd[1]);
        exit(0);
    }

    close(fd[1]);
    char buffer[100];
    while(1)
    {
        ssize_t n = read(fd[0], buffer, sizeof(buffer)-1);
        if(n <= 0)
            break;
        buffer[n] = '\0';
        std::cout << "recieved :" << buffer << std::endl;
    }

    close(fd[0]);
    waitpid(pid, NULL, 0);
    return;
}

int main(int argv, char **argc)
{
    // CGIHandler cgi;
    // cgi.buildEnvironment();
    // HTTPHandler han;
    // han.getDirectotyListing("./www", "");
    // forkExample();
    
    if(argv != 2)
        return 1;

    Config config;
    config.loadFile("./config.config");
    std::vector<ConfigServer>  configServer = config.parseConfig();
    config.setServer(configServer);
    

    Server server(std::atoi(argc[1]));
    // Client client;
    std::vector<struct pollfd> pollfds;
    std::map<int, HTTPRequest> clientRequests;
    std::map<int, ClientConnection> clients;
    
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
                // std::cout << "2" << std::endl;
                int fd = pollfds[i].fd;
                if(fd == server.getSocket())
                {
                    // std::cout << "3" << std::endl;
                    int clientFd = server.acceptClient();
                    if(clientFd >= 0)
                    {
                        // std::cout << "4" << std::endl;
                        // client.setFd(clientFd);
                        server.setNonBlocking(clientFd);
                        insertPollfds(pollfds, clientFd);
                        // clientRequests[clientFd] = HTTPRequest(); // REMOVE
                        ClientConnection ccon;
                        ccon.bytesSent = 0;
                        clients[clientFd] = ccon;
                    }
                }
                else
                {
                    // std::cout << "5" << std::endl;
                    // client.readData();
                    char buffer[BUFFERSIZE];
                    ssize_t bytesRead = recv(fd, buffer, BUFFERSIZE, 0);
                    
                    if(bytesRead > 0)
                    {
                        std::cout << "6" << std::endl;
                        // handle recieved data
                        // clientRequests[fd].appendData(buffer, bytesRead);// REMOVE
                        clients[fd].request.appendData(buffer, bytesRead);
                        // if(clientRequests[fd].isComplete(clientRequests[fd].getData())) // REMOVE
                        if(clients[fd].request.isComplete(clients[fd].request.getData()))
                        {
                            // clientRequests[fd].parseAll(clientRequests[fd].getData()); // REMOVE
                            clients[fd].request.parseAll(clients[fd].request.getData());
                            // std::cout << "Methode: " << clientRequests[fd].getMethode();
                            // std::cout << "\nPath: " << clientRequests[fd].getPath();
                            // std::cout << "\nVersion: " << clientRequests[fd].getVersion();
                            // std::cout << "\nBody: " << clientRequests[fd].getBody() << "\n\n";
                            
                            int serverPort = config.checkServerExists(server.getPort());
                            if(serverPort < 0)
                            {
                                std::cout << "INTERNAL ERROR. CONFIG PORT\n" << std::endl;
                                
                                // internal error if port and config port does not match
                                HTTPResponse httpRes;
                                httpRes.setStatus(500, httpRes.reasonPhrase(500));
                                httpRes.setBody("NO PORT\n");
                                std::string response = httpRes.response();
                                send(fd, response.c_str(), response.size(), 0);
                                close(fd);
                                pollfds.erase(pollfds.begin() + i);
                                // clientRequests.erase(fd); // REMOVE
                                clients.erase(fd);
                                i--;
                                continue;
                            }
                            std::cout << "6 a" << std::endl;
                            
                            std::string response;
                            CGIHandler  cgi;
                            ConfigServer cser = config.getServer(serverPort);
                            
                            if(cgi.isCGI(clients[fd].request.getPath()))
                            {
                                std::cout << "IN CGI" << std::endl;
                                cgi.setRequest(clients[fd].request);
                                cgi.setConfigServer(cser);
                                cgi.handleCGI();
                                response = cgi.getResponse();
                            }
                            else
                            {
                                std::cout << "IN HTTP HANDLER" << std::endl;
                                HTTPHandler han;
                                han.setRequest(clients[fd].request);
                                han.setConfigServer(cser);
                                han.handleRequest();
                                response = han.getResponse();
                            }
                            
                            clients[fd].response = response;
                            clients[fd].bytesSent = 0;
                            
                            // bitwise operation for switch states. stop reading, start writing
                            pollfds[fd].events &= ~POLLIN;
                            pollfds[i].events |= POLLOUT;
                            // send(fd, response.c_str(), response.size(), 0);
                            std::cout << "6 b" << std::endl;
                            // close(fd);
                            // pollfds.erase(pollfds.begin() + i);
                            // // clientRequests.erase(fd); // REMOVE
                            // clients.erase(fd);
                        }      
                        else
                            std::cout << "7" << std::endl;          
                    }
                    else if(bytesRead == 0)
                    {
                        // client closed the connection
                        std::cout << "Client has closed the connection" << std::endl;
                        close(fd); 
                        clients.erase(fd);
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
            if(pollfds[i].revents & POLLOUT) 
            {
                std::cout << "8" << std::endl;
                int fd = pollfds[i].fd;
                ClientConnection &cc = clients[fd];
                
                size_t remaining = cc.response.size() - cc.bytesSent;
                ssize_t sent = send(fd, cc.response.c_str() + cc.bytesSent, remaining, 0);

                if(sent > 0)
                    cc.bytesSent += sent;

                if(cc.bytesSent == cc.response.size())
                {
                    close(fd);
                    clients.erase(fd);
                    pollfds.erase(pollfds.begin() + i);
                    i--;
                }
                std::cout << "9" << std::endl;
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
