#include "pch.h"
#include "Server.h"
#include "IPUtils.h"
#include "Messages.h"
#include "ConnectionServer.h"

namespace GNAT {
	Server::Server() {
		SetConsoleTitleA(("Server [" + std::to_string(PORT) + "]").c_str());

		GNAT_Log::init_server();

		int wsaStartupCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsaStartupCode != 0) {
			errorFlag = WINSOCK_STARTUP_FAIL;
			SERVER_LOG_ERROR("Failed to startup Winsock starting server");
			return;
		}
		SERVER_LOG_INFO("Winsock startup success");

		sockAddr.sin_port = htons(PORT);
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

		if (bind(serverSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
			errorFlag = BINDING_SOCKET_FAIL;
			SERVER_LOG_ERROR("Failed to bind socket with server");
			return;
		}
		SERVER_LOG_INFO("Successfully binded server to socket on port: " + std::to_string(PORT));

		int val = 64 * 1024;
		setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));

		listen(serverSocket, 1000);
		SERVER_LOG_INFO("Server listen start");
	}

	Server::~Server() {
		SERVER_LOG_INFO("Closing server and cleaning up winsock...");
		closesocket(serverSocket);
		WSACleanup();
		SERVER_LOG_INFO("Winsock cleanup done. Server shutdown");
	}

	void Server::clearMessageBuffer() {
		ZeroMemory(messageBuffer, MESSAGE_BUFFER_SIZE);
	}

	void Server::startConnectionServer() {
		ConnectionServer* connServer = new ConnectionServer();

		SERVER_LOG_INFO("Starting Winsock...");
		connServer->initializeWinSock();

		SERVER_LOG_INFO("Establishing TCP Connection..");
		connServer->establishTCPConnection();

		SERVER_LOG_INFO("Killing Connection Server..");
		delete connServer;
	}

	void Server::openToClientConnection() {
		int currentClientCount = 0;

		clientIPList.reserve(TARGET_CLIENT_COUNT);

		SERVER_LOG_INFO("Searching for " + std::to_string(TARGET_CLIENT_COUNT) + " clients: ");
		while (currentClientCount < TARGET_CLIENT_COUNT) {
			SOCKADDR_IN clientAddr;
			int clientSize = sizeof(clientAddr);

			clearMessageBuffer();

			int bytesReceived = recvfrom(serverSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientSize);

			if (Messages::codesMatch(messageBuffer, bytesReceived, Messages::JOIN_REQ)) {
				Messages::dataByte thisClientsID(ClientNode::getNextNodeID());

				const int messageLen = MESSAGE_LENGTH + 1; // Join_Ack + ID
				char message[messageLen];

				strcpy_s(message, MESSAGE_LENGTH, Messages::JOIN_ACC);
				message[MESSAGE_LENGTH] = thisClientsID.signedByte;

				int bytesSent = 0;

				try {
					bytesSent = IP_Utils::sendMessage(serverSocket, clientAddr, message, messageLen);
				} catch (...) {
					SERVER_LOG_ERROR("Exception occurred when sending Join_Ack.");
				}

				if (bytesSent <= 0) {
					SERVER_LOG_ERROR("Failed to send Join_Ack. Error code: " + std::to_string(WSAGetLastError()));

					continue;
				}

				clientIPList.emplace_back(clientAddr);

				SERVER_LOG_INFO("    [" + std::to_string(currentClientCount) + "/" + std::to_string(TARGET_CLIENT_COUNT) + "] client found: " + clientIPList.at(currentClientCount).to_string());
				++currentClientCount;
			}

			Sleep(1);
		}

		// TODD: Broadcast Initial state and details to clients

		// Loop through each client node IDs
		// send a value to each one. can be the same eg 0
	}

	bool Server::startServer() {
		if (threadsRunning) {
			// TODO: Log info
			return false;
		}
		threadsRunning = true;

		//sendUpdates = std::thread(broadcastState);
		//recvUpdates = std::thread(startListen);

		return true;
	}

	bool Server::stopServer() {
		if (!threadsRunning) {
			// Log info
			return false;
		}

		threadsRunning = false;

		// Stop Threads somehow

		return true;
	}

	void Server::broadcastState() {
		const int MSG_LENGTH = MESSAGE_LENGTH + (TARGET_CLIENT_COUNT * 2);
		char updateMsg[MSG_LENGTH];

		while (true) {
			strcpy_s(updateMsg, MESSAGE_LENGTH, Messages::CURRENT_STATE);

			int clinetStepper = 0;
			for (int i = MESSAGE_LENGTH; clinetStepper < TARGET_CLIENT_COUNT; i += 2, ++clinetStepper) {
				ClientNode* thisClient = &clientIPList.at(clinetStepper);

				Messages::dataByte thisID(thisClient->getNodeID());
				const char thisVal = thisClient->getUpdateValue();

				updateMsg[i] = thisID.signedByte;
				updateMsg[i + 1] = thisVal;
			}


			// Broadcast Message
			for (int i = 0; i < TARGET_CLIENT_COUNT; ++i) {
				IP_Utils::sendMessage(serverSocket, clientIPList.at(i).getClient(), updateMsg, MSG_LENGTH);
			}

			Sleep(SEND_DELAY);
		}
	}

	void Server::startListen() {
		while (true) {
			SOCKADDR_IN clientAddr;
			int clientSize = sizeof(clientAddr);

			clearMessageBuffer();

			int bytesReceived = recvfrom(serverSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientSize);

			if (bytesReceived > 0) {
				try {
					// TODO
				}
				catch (...) {

				}
			}

			Sleep(1);
		}
	}

	const int Server::getErrorCode() const {
		return errorFlag;
	}
}