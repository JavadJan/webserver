#include "../include/Server.hpp"
#include "../include/Config.hpp"
#include "../include/Request.hpp"
#include <fcntl.h>     // For non-blocking sockets
#include <arpa/inet.h> // For inet_pton

// ---------------------- Constructor ----------------------
// Initialize the server using values from Config
Server::Server(Config conf)
    : _port(conf.getPort()), client_len(sizeof(client_addr)),
      server_fd(-1), client_fd(-1), poll_count(1)
{
    // Zero out the server and client address structures
    memset(&server_addr, 0, sizeof(server_addr));
    memset(&client_addr, 0, sizeof(client_addr));

    // Set address family (IPv4)
    server_addr.sin_family = AF_INET;

    // Convert string IP from Config to binary format
    if (inet_pton(AF_INET, conf.getHost().c_str(), &server_addr.sin_addr) <= 0)
    {
        std::cerr << "[Server] Invalid host IP, using 127.0.0.1" << std::endl;
        server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    // Set server port (network byte order)
    server_addr.sin_port = htons(_port);

    // Initialize pollfds vector with only the server socket for now
    poll_fds.clear();
}

// ---------------------- Create Socket, Bind, Listen ----------------------
int Server::create_socket_bind()
{
    // Create a TCP socket (IPv4)
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0)
    {
        std::cerr << "[Server] Socket error: " << strerror(errno) << std::endl;
        return -1;
    }

    // Allow reuse of the address (avoid "Address already in use" errors)
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        std::cerr << "[Server] Setsockopt error: " << strerror(errno) << std::endl;
        close(server_fd);
        return -1;
    }

    // Bind the socket to the configured IP and port
    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        std::cerr << "[Server] Bind error: " << strerror(errno) << std::endl;
        close(server_fd);
        return -1;
    }

    // Start listening with a backlog of 10 connections
    if (listen(server_fd, 10) < 0)
    {
        std::cerr << "[Server] Listen error: " << strerror(errno) << std::endl;
        close(server_fd);
        return -1;
    }

    std::cout << "[Server] Listening on "
              << inet_ntoa(server_addr.sin_addr) << ":" << _port << std::endl;

    // Add server_fd to poll_fds for monitoring
    pollfd serverPoll;
    serverPoll.fd = server_fd;
    serverPoll.events = POLLIN; // Monitor for incoming connections
    serverPoll.revents = 0;
    poll_fds.push_back(serverPoll);

    return 0;
}

// ---------------------- Accept New Client ----------------------
void Server::accept_new_connection()
{
    // Accept incoming connection
    socklen_t addr_len = sizeof(client_addr);
    int new_client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &addr_len);

    if (new_client_fd < 0)
    {
        std::cerr << "[Server] Accept error: " << strerror(errno) << std::endl;
        return;
    }

    // Set the new client socket to non-blocking
    int flags = fcntl(new_client_fd, F_GETFL, 0);
    fcntl(new_client_fd, F_SETFL, flags | O_NONBLOCK);

    // Add the new client to poll_fds
    add_to_poll_fds(new_client_fd);

    std::cout << "[Server] Accepted new client: fd=" << new_client_fd << std::endl;

    // Send a welcome message
    std::string welcome = "Welcome! You are client [" + std::to_string(new_client_fd) + "]\n";
    send(new_client_fd, welcome.c_str(), welcome.size(), 0);
}

// ---------------------- Read Data from a Client ----------------------
void Server::read_data_from_socket(int i)
{
    int sender_fd = poll_fds[i].fd;
    char buffer[2048];
    memset(buffer, 0, sizeof(buffer));

    int bytes = recv(sender_fd, buffer, sizeof(buffer), 0);

    if (bytes <= 0)
    {
        // Client disconnected or error occurred
        if (bytes == 0)
            std::cout << "[Server] Client fd=" << sender_fd << " disconnected." << std::endl;
        else
            std::cerr << "[Server] Recv error fd=" << sender_fd << ": " << strerror(errno) << std::endl;

        close(sender_fd);
        del_from_poll_fds(i);
        return;
    }

    std::string raw(buffer,bytes);
    std::cout << "[Server] Received from fd=" << sender_fd << ": " << raw << std::endl;

    // -----------------------------------------
    // Step 1: check if request is complete
    // -----------------------------------------
    size_t header_end = raw.find("\r\n\r\n");
    if (header_end == std::string::npos)
    {
        std::cout << "[Server] Incomplete request from fd=" << sender_fd << std::endl;
        return; // Wait for more data
    }
    
    // Split head/body
    std::string headerPart = raw.substr(0, header_end);
    std::string bodyPart = raw.substr(header_end + 4);

    Request req;

    // ---------------------------
    // Step 2: Parse request-line
    // ---------------------------

    size_t line_end = headerPart.find("\r\n");
    if (line_end == std::string ::npos)
    {
        std::cerr << "[Server] Bad request-line\n";
        return;
    }

    std::string requestLine = headerPart.substr(0, line_end);
    std::istringstream rl (requestLine);

    rl >> req.method >> req.target >> req.httpVersion ;

}

// ---------------------- Poll fd Management ----------------------
void Server::add_to_poll_fds(int new_fd)
{
    pollfd p;
    p.fd = new_fd;
    p.events = POLLIN;
    p.revents = 0;
    poll_fds.push_back(p);
}

void Server::del_from_poll_fds(int i)
{
    poll_fds[i] = poll_fds.back();
    poll_fds.pop_back();
}

// ---------------------- Main Server Loop ----------------------
void Server::run()
{
    if (create_socket_bind() < 0)
        throw ExceptionServer();

    while (true)
    {
        // Reset revents before polling
        for (size_t i = 0; i < poll_fds.size(); i++)
            poll_fds[i].revents = 0;

        int ret = poll(poll_fds.data(), poll_fds.size(), 2000); // 2 second timeout

        if (ret < 0)
        {
            std::cerr << "[Server] Poll error: " << strerror(errno) << std::endl;
            throw ExceptionServer();
        }
        else if (ret == 0)
        {
            // Timeout: no activity
            std::cout << "[Server] Listening..." << std::endl;
            continue;
        }

        // Check all fds for events
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            if (!(poll_fds[i].revents & POLLIN))
                continue;

            if (poll_fds[i].fd == server_fd)
                accept_new_connection();
            else
                read_data_from_socket(i);
        }
    }
}

// ---------------------- Exception ----------------------
const char *Server::ExceptionServer::what() const throw()
{
    return "Server failed to start!";
}
