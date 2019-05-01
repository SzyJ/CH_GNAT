#include "pch.h"
#include "ClientNode.h"
#include <time.h>

class GameClient {
private:
	time_t timeSinceLastUpdate;

	// Winsock Values
	SOCKET clientSocket;
	SOCKADDR_IN serverHint;
	SOCKADDR_IN clientHint;
	
	// Other Peers
	std::vector<ClientNode*>* clientIPList = NULL;

	// State
	byte thisID = 0;
	bool threadsRunning = false;

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

	byte getID();
	void setID(byte newID);
};