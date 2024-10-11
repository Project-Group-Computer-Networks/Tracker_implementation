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
        // sample Annnouncement:<8-byte Connection ID><4-byte Action (1 for announce)><4-byte Transaction ID><20-byte Info_hash><20-byte Peer ID><8-byte Downloaded><8-byte Left><8-byte Uploaded><4-byte Event><4-byte IP address><4-byte Key><4-byte Num Want><2-byte Port>
        //  Convert network byte order to host byte order where necessary
        req.connection_id = ntohl(req.connection_id);
        req.action = ntohl(req.action);
        req.transaction_id = ntohl(req.transaction_id);
        req.downloaded = ntohll(req.downloaded); // Assuming you have a ntohll for 64-bit
        req.left = ntohll(req.left);
        req.uploaded = ntohll(req.uploaded);
        req.event = ntohl(req.event);
        req.ip_address = ntohl(req.ip_address); // may need to change ntohl
        req.key = ntohl(req.key);
        req.num_want = ntohl(req.num_want);
        req.port = ntohs(req.port);

        // Extract info_hash and peer_id
        std::string info_hash(reinterpret_cast<char *>(req.info_hash), sizeof(req.info_hash));
        std::string peer_id(reinterpret_cast<char *>(req.peer_id), sizeof(req.peer_id));

        // Now use info_hash and peer_id for processing
        std::vector<Peer> peers = retrievePeersFromDatabase(info_hash);

        // sample response:<4-byte Action (1 for announce)><4-byte Transaction ID><Interval><Leechers><Seeders><Peer List (IP:Port)>
        //  Create a binary response
        std::vector<uint8_t> response;

        // Add the 4-byte action field
        uint32_t action_response = htonl(1); // 1 = announce response
        // std::cout << "action_response type: " << action_response << " " << typeid(action_response).name() << std::endl; // Ignore debugging statement
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&action_response), reinterpret_cast<uint8_t *>(&action_response) + sizeof(action_response));

        // Add the 4-byte transaction_id field
        uint32_t tran = htonl(req.transaction_id);
        response.insert(response.end(), reinterpret_cast<uint8_t *>(&tran), reinterpret_cast<uint8_t *>(&tran) + sizeof(tran));
        // std::cout << "transaction_id type: " << tran << " " << typeid(tran).name() << std::endl; // Ignore debugging statement
        // Add Interval, Leechers, Seeders (dummy values here)
        uint32_t interval = htonl(1800); // 1800 seconds for example
        uint32_t leechers = htonl(0);
        uint32_t seeders = htonl(0);
        // std::cout << "interval type: " << interval << " " << typeid(interval).name() << std::endl; // Ignore debugging statement
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
        // debugging response
        for (size_t i = 0; i < response.size(); ++i)
        {
            printf("%02x ", (unsigned char)response[i]);
        }
        printf("\n");
        // Send the response as a char array
        sendto(sockfd, reinterpret_cast<char *>(response.data()), response.size(), 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing announce request: " << e.what() << std::endl;
    }
}
