#pragma once
#include "pch.h"

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
	/*
	 * Sends Join Request to server
	 * 
	 * returns: 
	 *          The ID returned from the server if successful.
	 *          Negative value otherwise
	 */
	int sendJoinRequest();
};