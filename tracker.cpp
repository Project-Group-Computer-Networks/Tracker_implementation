#include "socket_util.h"
using namespace std;


void InteractWithClient(int clientSocket, vector<int>& clients) {
    cout << "Client connected" << endl;
    char buffer[4096];
    //sockaddr_in clientAddr;
    //socklen_t addrLen=sizeof(clientAddr);
    while (true) {
        //int b = recvfrom(clientSocket, buffer, sizeof(buffer), 0,(sockaddr*)&clientAddr,&addrLen);
        int b = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (b <= 0) {
            cout << "Client disconnected" << endl;
            break;
        }
        string message(buffer, b);
        cout << "Message from client: " << message << endl;

        for (auto client : clients) {
            if (client != clientSocket) {
                send(client, message.c_str(), message.length(), 0);
            }
        }
    }

    auto it = std::find(clients.begin(), clients.end(), clientSocket);
    if (it != clients.end()) clients.erase(it);
    close(clientSocket); // Close socket with `close()` in Linux
}

int main()
{
    cout << "Server" << endl;
    //int listenSocket=createUDPIPv4Socket();
    int listenSocket=createTCPIPv4Socket();
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
        /*incase of UDP server, we wont need accept(), listen() and use sendto(), recvfrom(). Also we must use mutex for threading*/
    vector<int> clients;

    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            cout << "Invalid client" << endl;
            continue;
        }

        clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket,ref(clients));
        t1.detach();
    }
    return 0;
}