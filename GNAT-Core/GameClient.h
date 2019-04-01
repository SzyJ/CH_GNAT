#include "pch.h"
#include "ClientNode.h"

class GameClient {
private:
	// Winsock Values
	SOCKET clientSocket;
	SOCKADDR_IN serverHint;
	SOCKADDR_IN clientHint;
	
	// Other Peers
	std::vector<ClientNode*>* clientIPList = NULL;

	// State
	char thisVal;

	// Helpers
	char checkForUserInput();
	bool sendToServer(const char* message, const int messageLen, const char* onErrorMsg = nullptr);

	// Thread Loop
	void ListenForUpdates();
	void ListenForKeyboard();

public:
	GameClient();
	~GameClient();

	int initializeWinSock();
	int startClient();
};