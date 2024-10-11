#include "udp_server.h"
#include "peer_manager.h"
#include <iostream>
#include <cstring>
#include <arpa/inet.h>

#define PORT 6969
#define MAX_BUFFER_SIZE 1024

void startUDPServer() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    char buffer[MAX_BUFFER_SIZE];

    // Create a UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        std::cerr << "Socket creation failed.\n";
        exit(EXIT_FAILURE);
    }

    // Set up server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind the socket to the port
    if (bind(sockfd, (const struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Bind failed.\n";
        exit(EXIT_FAILURE);
    }

    socklen_t len = sizeof(clientAddr);
    std::cout<<"Server listening on port "<<PORT<<std::endl;
    while (true) {
        // Receive data from clients
        int n = recvfrom(sockfd, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr*)&clientAddr, &len);
        buffer[n] = '\0';
        std::cout<<"Received message: "<<buffer<<std::endl;
        processAnnounceRequest(buffer, n, sockfd, clientAddr);
    }
}
