#include "pch.h"

#define HIGHVERSION 2
#define LOWVERSION 2
#define SERVER_ADDR "127.0.0.1"
#define SERVER_PORT 45000

class GameClient {
private:
	SOCKET clientSocket;
	SOCKADDR_IN serverHint;
	SOCKADDR_IN clientHint;
	



	
	int otherSize;

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void ListenForUpdates();

public:
	GameClient();
	~GameClient();

	int initializeWinsock();
	int startClient();
};