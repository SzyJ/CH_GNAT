#pragma once
#include "pch.h"
#include "ClientNode.h"

class ConnectionServer {
private:
	WSAData wsaData;
	SOCKET listenSock;
	SOCKADDR_IN hint;
	
	ClientNode* thisClient = nullptr;

	std::map<SOCKET, ClientNode*> socketMap;

	std::string getAddressAsString(SOCKADDR_IN addr);

public:
	ConnectionServer();

	int initializeWinSock();
	int establishTCPConnection();

	int addLocalhostAsClientOnPort(USHORT udpPort);

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