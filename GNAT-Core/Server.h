#pragma once
#include "pch.h"
#include "ClientNode.h"
#include "ErrorCodes.h"

namespace GNAT {
	class Server {
	private:
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		const static int TARGET_CLIENT_COUNT = 4;


		WSADATA wsaData;
		const static int PORT = 54000;
		const static int MESSAGE_BUFFER_SIZE = 1024;
		char messageBuffer[MESSAGE_BUFFER_SIZE];
		const static int SEND_DELAY = 1000;
		std::vector<ClientNode> clientIPList;
		SOCKADDR_IN sockAddr;
		SOCKET serverSocket;

		std::thread sendUpdates;
		std::thread recvUpdates;

		bool threadsRunning;

		void clearMessageBuffer();

		/*
		 * Send the value of each client to each client.
		 * SEND_DELAY defines the wait between each transmission
		 */
		void broadcastState();

		void startListen();

	public:
		Server();
		~Server();

		/* 
		 * Opens a port to listen for signals on. When a "join"
		 * message is received, add the sender of the message to
		 * the list of clients.
		 */
		void openToClientConnection();
		void startConnectionServer();
		bool startServer();
		bool stopServer();

		const int getErrorCode() const;
	};
}