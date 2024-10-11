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
// Struct to represent the announce request
struct AnnounceRequest
{
    uint64_t connection_id;
    uint32_t action;
    uint32_t transaction_id;
    uint8_t info_hash[20];
    uint8_t peer_id[20];
    uint64_t downloaded;
    uint64_t left;
    uint64_t uploaded;
    uint32_t event;
    uint32_t ip_address;
    uint32_t key;
    uint32_t num_want;
    uint16_t port;
};
// Define ntohll function
uint64_t ntohll(uint64_t value);
// Function to process the announce request from a client
void processAnnounceRequest(char *buffer, int length, int sockfd, sockaddr_in &clientAddr);

#endif // PEER_MANAGER_H
