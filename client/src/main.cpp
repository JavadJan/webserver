#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

int main() {
    int sockfd;
    struct sockaddr_in server_addr;
    char buffer[16384]; // 16KB buffer
    std::string message;

    // Build a large header > 16KB
    message = "GET / HTTP/1.1\r\nHost: localhost\r\n";
    for (int i = 0; i < 1700; ++i) // roughly 17KB of header
    {
        message += "X-Test";
        message += std::to_string(i);
        message += ": AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\r\n";
    }
    message += "\r\n";

    // 1. Create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) { perror("socket"); return 1; }

    // 2. Connect
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(4243); // match your server
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("connect");
        return 1;
    }

    // 3. Send message
    if (send(sockfd, message.c_str(), message.size(), 0) < 0) {
        perror("send");
        return 1;
    }
    std::cout << "[CLIENT] Sent " << message.size() << " bytes\n";

    // 4. Receive reply
    int n = recv(sockfd, buffer, sizeof(buffer)-1, 0);
    if (n > 0) {
        buffer[n] = '\0';
        std::cout << "[CLIENT] Received:\n" << buffer << "\n";
    } else {
        perror("recv");
    }

    // 5. Close
    close(sockfd);
    return 0;
}
