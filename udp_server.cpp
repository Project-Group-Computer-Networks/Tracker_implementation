#include "socket_util.h"
using namespace std;

#define PORT 6969
#define PROTOCOL_ID 0x41727101980
#define ACTION_CONNECT 0
#define ACTION_RESPONSE_CONNECT 1
// changed from 0
// Convert to network byte order
uint64_t htonll(uint64_t hostlonglong) {
    return ((uint64_t)htonl(hostlonglong & 0xFFFFFFFF) << 32) | htonl(hostlonglong >> 32);
}

// Convert from network byte order
uint64_t ntohll(uint64_t netlonglong) {
    return ((uint64_t)ntohl(netlonglong & 0xFFFFFFFF) << 32) | ntohl(netlonglong >> 32);
}

uint64_t generateConnectionID() {
    return ((uint64_t)rand() << 32) | rand();
}

#pragma pack(push, 1)
struct ConnectRequest {
    uint64_t protocol_id;
    uint32_t action;
    uint32_t transaction_id;
};

struct ConnectResponse {
    uint32_t action;
    uint32_t transaction_id;
    uint64_t connection_id;
};
#pragma pack(pop)

int main()
{
    cout << "Server" << endl;
    int listenSocket=createUDPIPv4Socket();
    if (listenSocket < 0) {
        cout << "Socket creation failed" << endl;
        return 0;
    }

    sockaddr_in trackeraddr=createIPv4Address("0.0.0.0",PORT);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&trackeraddr), sizeof(trackeraddr)) < 0) {
        cout << "Bind error" << endl;
        close(listenSocket);
        return 0;
    }

        
        char buffer[4096];
        sockaddr_in clientAddr;
        socklen_t addrLen=sizeof(clientAddr);
        while (true) {
        int bytesRcvd = recvfrom(listenSocket, buffer, sizeof(buffer), 0,(sockaddr*)&clientAddr,&addrLen);

        string message(buffer, bytesRcvd);
        cout << "Message from client: " << message << endl;

        cout << "Client connected" << endl;
        if (bytesRcvd <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }

        if (bytesRcvd>= sizeof(ConnectRequest)) {
            ConnectRequest* request = reinterpret_cast<ConnectRequest*>(buffer);

            if (request->protocol_id == PROTOCOL_ID && request->action == ACTION_CONNECT) {
                cout << "Received a valid connect request" << endl;

                
                uint64_t connection_id = generateConnectionID();
                
                printf("Generated connnection id: %llu\n", (unsigned long long)connection_id);

                // cout << "Generated connection ID: " << connection_id << endl;

                ConnectResponse response;
                response.action = ACTION_RESPONSE_CONNECT;
                response.transaction_id = request->transaction_id;
                response.connection_id = connection_id;
                response.connection_id = htonll(connection_id); // Convert to network byte order

                int bytesSent = sendto(listenSocket, reinterpret_cast<char*>(&response), sizeof(response), 0, (sockaddr*)&clientAddr, addrLen);
                if (bytesSent <= 0) {
                    cout << "Failed to send response" << endl;
                } else {
                    cout << "Sent connect response to client" << endl;
                }
            }
             else {
                cout << "Invalid protocol ID or action in request" << endl;
            }
        } 
        else {
            cout << "Invalid connect request received" << endl;
        }

        
    }
    close(listenSocket);
    return 0;
}