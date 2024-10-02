#include "socket_util.h"

int createUDPIPv4Socket()
{
    int s=socket(AF_INET,SOCK_DGRAM,0);
    return s;
}
int createTCPIPv4Socket()
{
    int s=socket(AF_INET,SOCK_STREAM,0);
    return s;
}
sockaddr_in createIPv4Address(const char* ip, int port) {
    sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(port);
    
    if (strlen(ip) == 0) {
        address.sin_addr.s_addr = INADDR_ANY;
    } else {
        inet_pton(AF_INET, ip, &address.sin_addr);
    }
    
    return address;
}