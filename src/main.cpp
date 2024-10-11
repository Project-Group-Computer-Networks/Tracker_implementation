#include "udp_server.h"
#include "db_manager.h"
#include "peer_manager.h"

int main() {
    // Initialize the database
    initializeDatabase();

    // Start the UDP server
    startUDPServer();

    return 0;
}
