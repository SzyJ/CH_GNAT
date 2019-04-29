#pragma once
#include "pch.h"
#include "ClientNode.h"

class ConnectionClient {
private:
	SOCKET clientSocket = INVALID_SOCKET;
	addrinfo* server_addr_result = nullptr;

	byte clientID = 0;
	bool connectionEstablished = false;

	std::vector<ClientNode*>* clientIPList = nullptr;

public:
	ConnectionClient();
	~ConnectionClient();

	int initializeWinSock();
	int connectToServer();
	/*
	 * Sends Join Request to server
	 * 
	 * returns: 
	 *          The ID returned from the server if successful.
	 *          Negative value otherwise
	 */
	int sendJoinRequest(u_short udpPort);

	int listenForPeerInfo();

	std::vector<ClientNode*>* getClientList();
};