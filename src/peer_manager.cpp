#include "peer_manager.h"
#include "db_manager.h"
#include <iostream>
#include <arpa/inet.h>
#include <vector>
#include <cstring>
#include <memory>
#include "custom_bencoder.h"

void processAnnounceRequest(char *buffer, int length, int sockfd, sockaddr_in &clientAddr)
{
    std::string announceRequest(buffer, length); // Convert C-style buffer to std::string
    std::string::const_iterator start = announceRequest.begin();

    try
    {
        // Use bencoder to parse the message (assuming it’s a dictionary)
        // std::unique_ptr<bencoding::bencode_base> parsed_message = bencoding::make_value(announceRequest, start);
        bencoding::bencode_dict bd;
        bd.decode(announceRequest, start);
        // Check if the parsed message is a dictionary

        // Extract the necessary fields (info_hash, peer_id, port)
        // auto info_hash_it = dict->find("info_hash");
        // auto peer_id_it = dict->find("peer_id");
        // auto port_it = dict->find("port");

        // Extract info_hash, peer_id, and port from the request
        bencoding::bencode_string info_hash, peer_id, port;

        bencoding::string_subs sa(bd["info_hash"]->encode());
        info_hash.decode(sa.str, sa.citer);
        std::string info_hash_str = info_hash.get();
        bencoding::string_subs sp(bd["port"]->encode());
        port.decode(sp.str, sp.citer);
        int port_str = std::stoi(port.get());
        bencoding::string_subs spi(bd["peer_id"]->encode());
        peer_id.decode(spi.str, spi.citer);
        std::string peer_id_str = peer_id.get();

        // std::string info_hash = dynamic_cast<bencoding::bencode_string*>(info_hash_it->second.get())->get();
        // std::string peer_id = dynamic_cast<bencoding::bencode_string*>(peer_id_it->second.get())->get();
        // int port = dynamic_cast<bencoding::bencode_integer*>(port_it->second.get())->get();

        // Debugging: Print extracted info (optional)
        // std::cout << "Received announce request:" << std::endl;
        // std::cout << "Info Hash: " << info_hash << std::endl;
        // std::cout << "Peer ID: " << peer_id << std::endl;
        // std::cout << "Port: " << port << std::endl;

        // Get the list of peers for the given info_hash
        std::vector<Peer> peers = retrievePeersFromDatabase(info_hash_str);

        // Prepare the response in bencoded format
        bencoding::bencode_dict response_dict;
        bencoding::bencode_list peers_list;

        for (const auto &peer : peers)
        {
            bencoding::bencode_dict peer_dict;
            // peer_dict.insert_or_assign<bencoding::bencode_string>("peer_id", peer.peer_id);
            peer_dict["peer_id"] = std::make_unique<bencoding::bencode_string>(peer.peer_id);
            // peer_dict.insert_or_assign<bencoding::bencode_string>("ip", peer.ip);
            peer_dict["ip"] = std::make_unique<bencoding::bencode_string>(peer.ip);
            // peer_dict.insert_or_assign<bencoding::bencode_integer>("port", peer.port);
            peer_dict["port"] = std::make_unique<bencoding::bencode_integer>(peer.port);

            peers_list.push_back<bencoding::bencode_dict>(std::move(peer_dict));
        }

        // Add the peers list to the response dictionary
        // response_dict.insert_or_assign<bencoding::bencode_list>("peers", std::move(peers_list));
        response_dict["peers"] = std::make_unique<bencoding::bencode_list>(std::move(peers_list));

        // Encode the response into a string
        std::string encoded_response = response_dict.encode();

        // Send the response back to the client
        sendPeerList(sockfd, clientAddr, encoded_response, peers);
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error processing announce request: " << e.what() << std::endl;
    }
}

void sendPeerList(int sockfd, sockaddr_in &clientAddr, std::string response, const std::vector<Peer> &peers)
{
    // std::string response = "d5:peersl";  // Example response

    // for (const Peer& peer : peers) {
    //     response += "6:" + peer.ip + std::to_string(peer.port);
    // }
    // response += "e";  // End the response

    // Send the response
    sendto(sockfd, response.c_str(), response.size(), 0, (const struct sockaddr *)&clientAddr, sizeof(clientAddr));
}