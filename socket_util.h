#ifndef SOCKETUTIL_H
#define SOCKETUTIL_H

#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <vector>
#include <algorithm>
#include <cstring>

sockaddr_in createIPv4Address(const char* ip,int port);
int createUDPIPv4Socket();
int createTCPIPv4Socket();

#endif