#include "socket_util.h"
using namespace std;
int main()
{
    cout << "Server" << endl;
    int listenSocket=createUDPIPv4Socket();
    if (listenSocket < 0) {
        cout << "Socket creation failed" << endl;
        return 0;
    }

    int trackport=12345;
    sockaddr_in trackeraddr=createIPv4Address("0.0.0.0",trackport);
    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&trackeraddr), sizeof(trackeraddr)) < 0) {
        cout << "Bind error" << endl;
        close(listenSocket);
        return 0;
    }

    if (listen(listenSocket, SOMAXCONN) < 0) {
        cout << "Listen error" << endl;
        close(listenSocket);
        return 0;
    }

    return 0;
}