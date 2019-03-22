#pragma once
#include "pch.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 45000

class ConnectionClinet {
private:
	SOCKET clientSocket;
	SOCKADDR_IN hint;

	bool connectionEstablished = false;

public:
	ConnectionClinet();
	~ConnectionClinet();

	int initializeWinSock();
	int connectToServer();
	int sendJoinRequest();
};