#include<socket_util.h>
#include "udp_server.h"
#include "db_manager.h"
#include "peer_manager.h"

int main() {
    initializeDatabase();
    startUDPServer();
    return 0;
}