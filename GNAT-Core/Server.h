#pragma once
#include "pch.h"
#include "ServerErrors.h"

namespace GNAT {
	class Server {
	private:
		int errorFlag = STARTUP_SUCCESSFUL;

		const static int PORT = 54000;
		const static int MESSAGE_BUFFER_SIZE = 1024;
		char messageBuffer[MESSAGE_BUFFER_SIZE];

		SOCKET serverSocket;

		bool serverRunning;

		void clearMessageBuffer();
		int sendMessage(SOCKADDR_IN receiver, std::string msg);

	public:
		Server();
		~Server();

		void startListen();

		const int getErrorCode() const;
	};
}