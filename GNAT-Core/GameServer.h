#include "pch.h"
#include "ClientNode.h"
#include <string>

class GameServer {
private:
	SOCKET serverSocket;
	SOCKADDR_IN serverHint;

	std::vector<ClientNode*>* clientIPList = NULL;

	void startStateBroadcast();

public:
	GameServer();
	~GameServer();

	bool setClientList(std::vector<ClientNode*>* clientIPList);

	int initializeWinSock();
	int startServer();
};