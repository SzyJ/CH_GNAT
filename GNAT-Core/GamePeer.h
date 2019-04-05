#pragma once
#include "pch.h"
#include "ClientNode.h"

class GamePeer {
private:
	// Winsock Values
	SOCKET peerSocket;
	SOCKADDR_IN peerHint;

	// Other Peers
	std::vector<ClientNode*>* peerIPList = NULL;

	// State
	char thisVal;

	// Helpers
	char checkForUserInput();
	bool sendToPeers(const char* message, const int messageLen, const char* onErrorMsg = nullptr);

	// Thread Loop
	void ListenForUpdates();
	void ListenForKeyboard();

public:
	GamePeer();
	~GamePeer();

	int initializeWinSock();
	int startClient();
};