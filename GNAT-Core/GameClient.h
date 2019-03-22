#include "pch.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 45000

class GameClient {
private:
	// Winsock Values
	SOCKET clientSocket;
	SOCKADDR_IN serverHint;
	SOCKADDR_IN clientHint;
	
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

	int initializeWinsock();
	int startClient();
};