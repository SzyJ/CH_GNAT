#pragma once
#include "pch.h"
#include "ClientNode.h"

class ConnectionServer {
private:
	WSAData wsaData;
	SOCKET listenSock;
	SOCKADDR_IN hint;
	
	std::vector<ClientNode*>* clientIPList;

public:
	ConnectionServer();

	int initializeWinSock();
	int establishTCPConnection();

	std::vector<ClientNode*>* getClientList();
};