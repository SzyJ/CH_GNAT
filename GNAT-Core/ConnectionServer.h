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

	/*
	 * Sends the IP Address and PORT of each client
	 * to each other client. This is used in P2P
	 * connections.
	 * Returns:
	 *          If successful, positive value repre-
	 *          senting the amount of clients.
	 *          Negative value represents error.
	 */
	int broadcastClientState();

	std::vector<ClientNode*>* getClientList();
};