#ifndef PEER_MANAGER_H
#define PEER_MANAGER_H

#include <string>
#include <vector>
#include <netinet/in.h> // For sockaddr_in

// Struct representing a peer
struct Peer
{
    std::string ip;      // Peer IP address
    int port;            // Peer port number
    std::string peer_id; // Peer ID
};

// Function to process the announce request from a client
void processAnnounceRequest(char *buffer, int length, int sockfd, sockaddr_in &clientAddr);

// Function to send the peer list back to the client
void sendPeerList(int sockfd, sockaddr_in &clientAddr, std::string response, const std::vector<Peer> &peers);

#endif // PEER_MANAGER_H
