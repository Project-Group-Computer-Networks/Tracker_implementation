#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <vector>
#include <cstdint> // for fixed-width integer types like uint32_t, uint16_t
#include "db_manager.h"

// Define ntohll function for 64-bit integers
uint64_t ntohll(uint64_t value)
{
    if (htonl(1) != 1)
    { // Check if the system is big-endian
        return value;
    }
    return ((uint64_t)ntohl(value & 0xFFFFFFFF) << 32) | ntohl(value >> 32);
}

void processAnnounceRequest(char *buffer, int length, int sockfd, sockaddr_in &clientAddr)
{
    try
    {
        // Check if buffer length matches the expected size
        if (length < sizeof(AnnounceRequest))
        {
            throw std::runtime_error("Invalid announce request length");
        }

        AnnounceRequest req;
        memcpy(&req, buffer, sizeof(AnnounceRequest));

        // Convert network byte order to host byte order where necessary
        req.connection_id = ntohl(req.connection_id);
        req.action = ntohl(req.action);
        req.transaction_id = ntohl(req.transaction_id);
        req.downloaded = ntohll(req.downloaded); // Assuming you have a ntohll for 64-bit
        req.left = ntohll(req.left);
        req.uploaded = ntohll(req.uploaded);
        req.event = ntohl(req.event);
        req.ip_address = ntohl(req.ip_address);
        req.key = ntohl(req.key);
        req.num_want = ntohl(req.num_want);
        req.port = ntohs(req.port);

        // Extract info_hash and peer_id
        std::string info_hash(reinterpret_cast<char *>(req.info_hash), sizeof(req.info_hash));
        std::string peer_id(reinterpret_cast<char *>(req.peer_id), sizeof(req.peer_id));

        // Now use info_hash and peer_id for processing
        std::vector<Peer> peers = retrievePeersFromDatabase(info_hash);

        // Create a binary response
        std::vector<uint8_t> response;

        // Add the 4-byte action field
        uint32_t action_response = htonl(1); // 1 = announce response
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&action_response), reinterpret_cast<uint8_t *>(&action_response) + sizeof(action_response));

        // Add the 4-byte transaction_id field
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&req.transaction_id), reinterpret_cast<uint8_t *>(&req.transaction_id) + sizeof(req.transaction_id));

        // Add Interval, Leechers, Seeders (dummy values here)
        uint32_t interval = htonl(1800); // 1800 seconds for example
        uint32_t leechers = htonl(0);
        uint32_t seeders = htonl(0);
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&interval), reinterpret_cast<uint8_t *>(&interval) + sizeof(interval));
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&leechers), reinterpret_cast<uint8_t *>(&leechers) + sizeof(leechers));
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&seeders), reinterpret_cast<uint8_t *>(&seeders) + sizeof(seeders));

        // Add peers in binary format (IP + Port)
        for (const auto &peer : peers)
        {
            uint32_t peer_ip = inet_addr(peer.ip.c_str()); // Convert IP to binary
            uint16_t peer_port = htons(peer.port);

            response.insert(response.end(), reinterpret_cast<uint8_t *>(&peer_ip), reinterpret_cast<uint8_t *>(&peer_ip) + sizeof(peer_ip));
            response.insert(response.end(), reinterpret_cast<uint8_t *>(&peer_port), reinterpret_cast<uint8_t *>(&peer_port) + sizeof(peer_port));
        }

        // Send the response as a char array
        sendto(sockfd, reinterpret_cast<char *>(response.data()), response.size(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing announce request: " << e.what() << std::endl;
    }
}
