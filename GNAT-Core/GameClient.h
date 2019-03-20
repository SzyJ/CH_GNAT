#include "pch.h"

class GameClient {
private:
	const static int BUFFERLENGTH = 1024;
	char buffer[BUFFERLENGTH];

	SOCKET connectSocket;
	SOCKADDR_IN otherAddr;
	int otherSize;

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void TaskRec();

public:
	int startClient();
};