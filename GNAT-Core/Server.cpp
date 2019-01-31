#include "pch.h"
#include "Server.h"
#include "IPUtils.h"
#include "Messages.h"

namespace GNAT {
	Server::Server() {
		SetConsoleTitleA(("Server [" + std::to_string(PORT) + "]").c_str());

		WSADATA wsaData;
		int wsaStartupCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsaStartupCode != 0) {
			errorFlag = WINSOCK_STARTUP_FAIL;
			SERVER_LOG_ERROR("Failed to startup Winsock starting server");
			return;
		}
		SERVER_LOG_INFO("Winsock startup success");

		SOCKADDR_IN sockAddr;
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
		setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));

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

	void Server::openToClientConnection() {
		int currentClientCount = 0;

		clientIPList.reserve(TARGET_CLIENT_COUNT);

		SERVER_LOG_INFO("Searching for " + std::to_string(TARGET_CLIENT_COUNT) + " clients: ");
		while (currentClientCount < TARGET_CLIENT_COUNT) {
			SOCKADDR_IN clientAddr;
			int clientSize = sizeof(clientAddr);

			clearMessageBuffer();

			int bytesReceived = recvfrom(serverSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientSize);

			if (bytesReceived > 0) {
				try {
					// Capture Message
					std::string receivedMsg = std::string(messageBuffer, messageBuffer + bytesReceived);

					if (receivedMsg == Messages::JOIN_REQ) {
						clientIPList.emplace_back(clientAddr);
						IP_Utils::sendMessage(serverSocket, clientAddr, Messages::JOIN_ACC + std::to_string(ClientNode::getLastNodeID()));
						SERVER_LOG_INFO("    [" + std::to_string(currentClientCount) + "/" + std::to_string(TARGET_CLIENT_COUNT) + "] client found: " + clientIPList.at(currentClientCount).to_string());
						++currentClientCount;
					}
				}
				catch (std::invalid_argument& e) {}
				catch (std::out_of_range& e) {}
				catch (...) {}
			}

			Sleep(1);
		}

		// TODD: Broadcast Initial state and details to clients
	}

	void Server::broadcastState() {
		const char* MESSAGE_TYPE = Messages::UPDATE.c_str();
		const int TYPE_LENGTH = 2;
		const int MSG_LENGTH = TYPE_LENGTH + TARGET_CLIENT_COUNT;

		char update[MSG_LENGTH];
		for (int i = 0; i < TYPE_LENGTH; ++i) {
			update[i] = MESSAGE_TYPE[i];
		}

		while (true) {
			// Build Update Message	
			for (int i = 0; i < TARGET_CLIENT_COUNT; ++i) {
				update[TYPE_LENGTH + i] = clientIPList.at(i).getUpdateValue();
			}

			// Broadcast Message
			for (int i = 0; i < clientIPList.size(); ++i) {
				IP_Utils::sendMessage(serverSocket, clientIPList.at(i).getClient(), update, MSG_LENGTH);
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
					// Capture Message
					std::string receivedMsg = std::string(messageBuffer, messageBuffer + bytesReceived);

					// Define the address and port
					std::string addr(IP_Utils::IP_STRING_LENGTH, ' ');
					USHORT port;
					IP_Utils::expandAddress(clientAddr, &addr, &port);

					// Send Message
					IP_Utils::sendMessage(serverSocket, clientAddr, "");
				}
				catch (std::invalid_argument& e) {}
				catch (std::out_of_range& e) {}
				catch (...) {}
			}

			Sleep(1);
		}
	}

	const int Server::getErrorCode() const {
		return errorFlag;
	}
}