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
int main()
{
    int listenSocket=socket(AF_INET,SOCK_DGRAM,0);//create a UDP socket for listening
    if (listenSocket < 0) {
        cout << "Socket creation failed" << endl;
        return 0;
    }

    return 0;
}