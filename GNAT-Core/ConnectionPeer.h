#pragma once
#include "pch.h"
#include "ClientNode.h"

class ConnectionPeer {
private:
	// Other Peers
	std::vector<ClientNode*>* peerIPList = NULL;

public:
	ConnectionPeer();
	~ConnectionPeer();

	int initializeWinSock();

	/*
	 * Sends Join Request to session host peer
	 *
	 * returns:
	 *          The ID returned from the server if successful.
	 *          Negative value otherwise
	 */
	int sendJoinRequest(u_short udpPort);
};