#include "pch.h"
#include <string>

class GameServer {
private:
	const int PORT = 54000;
	SOCKET serverSocket;
	std::map<int, SOCKADDR_IN> current;

	std::string NormalizedIPString(SOCKADDR_IN addr);
	void SendResponse(SOCKADDR_IN addr, SOCKADDR_IN receiver);

public:
	int startServer();
};