#ifndef DB_MANAGER_H
#define DB_MANAGER_H
#include <string>
#include <vector>
#include "peer_manager.h"

void initializeDatabase();
void storePeerInDatabase(const std::string& infoHash, const std::string& peerId, const std::string& ip, int port, int uploaded, int downloaded, int left);
std::vector<Peer> retrievePeersFromDatabase(const std::string& infoHash);
#endif