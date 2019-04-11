#pragma once
#include "pch.h"
#include "ClientNode.h"

class ConnectionClient {
private:
	SOCKET clientSocket;
	SOCKADDR_IN hint;

	byte clientID = 0;
	bool connectionEstablished = false;

	std::vector<ClientNode*>* clientIPList = nullptr;

public:
	ConnectionClient();
	~ConnectionClient();

	int initializeWinSock(const char* address, const USHORT port);
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