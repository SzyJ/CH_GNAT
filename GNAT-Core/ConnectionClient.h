#pragma once
#include "pch.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 45000

class ConnectionClinet {
private:
	WSAData wsaData;
	SOCKET clientSock;
	SOCKADDR_IN hint;

	bool connectionEstablished = false;

public:
	ConnectionClinet();

	int initializeWinSock();
	int connectToServer();
	int sendJoinRequest();
};