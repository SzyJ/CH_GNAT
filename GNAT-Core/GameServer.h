#include "pch.h"
#include "ClientNode.h"

class GameServer {
private:
	SOCKET serverSocket;
	SOCKADDR_IN serverHint;

	std::vector<ClientNode*>* clientIPList = NULL;

	bool threadsRunning = false;

	void startStateBroadcast();
	void startUpdateListen();

public:
	GameServer();
	~GameServer();

	bool setClientList(std::vector<ClientNode*>* clientIPList);

	int initializeWinSock();
	int startServer();
};