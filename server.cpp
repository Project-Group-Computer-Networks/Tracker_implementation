#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

using namespace std;

bool Initialize() {
    return true; // No WSAStartup equivalent needed for Linux
}

void InteractWithClient(int clientSocket, vector<int>& clients) {
    cout << "Client connected" << endl;
    char buffer[4096];

    while (true) {
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

int main() {
    if (!Initialize()) {
        cout << "Initialization failed" << endl;
        return 0;
    }

    cout << "Server" << endl;
    int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (listenSocket < 0) {
        cout << "Socket creation failed" << endl;
        return 0;
    }

    int port = 12345;
    sockaddr_in serveraddr;
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(port);
    inet_pton(AF_INET, "0.0.0.0", &serveraddr.sin_addr);

    if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) < 0) {
        cout << "Bind error" << endl;
        close(listenSocket);
        return 0;
    }

    if (listen(listenSocket, SOMAXCONN) < 0) {
        cout << "Listen error" << endl;
        close(listenSocket);
        return 0;
    }

    cout << "Server listening on port: " << port << endl;
    vector<int> clients;

    while (true) {
        int clientSocket = accept(listenSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            cout << "Invalid client" << endl;
            continue;
        }

        clients.push_back(clientSocket);
        thread t1(InteractWithClient, clientSocket, std::ref(clients));
        t1.detach();
    }

    close(listenSocket); // Close the listening socket
    return 0;
}
