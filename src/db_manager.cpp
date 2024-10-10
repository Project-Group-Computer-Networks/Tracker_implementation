#include "peer_manager.h"
#include "db_manager.h"
#include <sqlite3.h>
#include <iostream>
#include <vector>

sqlite3* db=nullptr;

std::vector<Peer> retrievePeersFromDatabase(const std::string& infoHash) {
    std::vector<Peer> peers;
    
    const char* selectSQL = "SELECT peer_id, ip, port FROM peers WHERE info_hash = ?";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, selectSQL, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return peers;
    }

    sqlite3_bind_text(stmt, 1, infoHash.c_str(), -1, SQLITE_STATIC);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        Peer peer;
        peer.peer_id = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
        peer.ip = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
        peer.port = sqlite3_column_int(stmt, 2);
        peers.push_back(peer);
    }

    sqlite3_finalize(stmt);
    return peers;
}

void initializeDatabase() {
    if (sqlite3_open("tracker.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        exit(1);
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS peers (
            info_hash TEXT NOT NULL,
            peer_id TEXT NOT NULL,
            ip TEXT NOT NULL,
            port INTEGER NOT NULL,
            uploaded INTEGER,
            downloaded INTEGER,
            left INTEGER
        );
    )";

    char* errMsg = 0;
    if (sqlite3_exec(db, createTableSQL, NULL, 0, &errMsg)) {
        std::cerr << "SQL error: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
}

void storePeerInDatabase(const std::string& infoHash, const std::string& peerId, const std::string& ip, int port, int uploaded, int downloaded, int left) {
    const char* insertSQL = "INSERT INTO peers (info_hash, peer_id, ip, port, uploaded, downloaded, left) VALUES (?, ?, ?, ?, ?, ?, ?)";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, 0) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    sqlite3_bind_text(stmt, 1, infoHash.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, peerId.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, ip.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 4, port);
    sqlite3_bind_int(stmt, 5, uploaded);
    sqlite3_bind_int(stmt, 6, downloaded);
    sqlite3_bind_int(stmt, 7, left);

    if (sqlite3_step(stmt) != SQLITE_DONE) {
        std::cerr << "Failed to insert data: " << sqlite3_errmsg(db) << std::endl;
    }

    sqlite3_finalize(stmt);
}
