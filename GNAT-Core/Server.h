#pragma once
#include "pch.h"
#include "ClientNode.h"
#include "ErrorCodes.h"

namespace GNAT {
	class Server {
	private:
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		const static int TARGET_CLIENT_COUNT = 3;

		const static int PORT = 54000;
		const static int MESSAGE_BUFFER_SIZE = 1024;
		char messageBuffer[MESSAGE_BUFFER_SIZE];

		const static int SEND_DELAY = 1000;

		std::vector<ClientNode> clientIPList;

		SOCKET serverSocket;

		bool serverRunning;

		void clearMessageBuffer();
		int sendMessage(SOCKADDR_IN receiver, std::string msg);
		int sendMessage(SOCKADDR_IN receiver, char* msg, int msgLength);

	public:
		Server();
		~Server();

		/* 
		 * Opens a port to listen for sygnals on. When a "join"
		 * message is received, add the sender of the message to
		 * the list of clients.
		 */
		void openToClientConnection();

		/*
		 * Send the value of each client to each client.
		 * SEND_DELAY defines the wait between each transmission
		 */
		void broadcastState();
		
		void startListen();

		const int getErrorCode() const;
	};
}