#include "socket_util.h"
using namespace std;

#define PORT 6969
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
        int b = recvfrom(listenSocket, buffer, sizeof(buffer), 0,(sockaddr*)&clientAddr,&addrLen);
        cout << "Client connected" << endl;
        if (b <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        string message(buffer, b);
        cout << "Message from client: " << message << endl;

    
        /*incase of UDP server, we wont need accept(), listen() and use sendto(), recvfrom(). Also we must use mutex for threading*/
    }
    return 0;
}