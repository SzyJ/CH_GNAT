#pragma once
#include "pch.h"
#include "ClientNode.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define HOST "127.0.0.1"
#define PORT 45000
#define FAMILY AF_INET
#define TYPE SOCK_STREAM
#define FLAGS AI_PASSIVE

class ConnectionServer {
private:
	WSAData wsaData;
	SOCKET listenSock;
	SOCKADDR_IN hint;
	
	std::vector<ClientNode> clientIPList;

public:
	ConnectionServer();

	int initializeWinSock();
	int establishTCPConnection();
};