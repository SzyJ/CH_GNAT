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
			LOG_ERROR("Failed to startup Winsock starting server");
			return;
		}
		LOG_INFO("WinSock startup success!");


		SOCKADDR_IN sockAddr;
		sockAddr.sin_port = htons(PORT);
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

		if (bind(serverSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
			errorFlag = BINDING_SOCKET_FAIL;
			LOG_ERROR("Failed to bind socket with server");
			return;
		}
		LOG_INFO("Successfully binded server to socket");

		int val = 64 * 1024;
		setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
		setsockopt(serverSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));

		listen(serverSocket, 1000);
	}

	Server::~Server() {
		closesocket(serverSocket);
		WSACleanup();
	}

	int Server::sendMessage(SOCKADDR_IN receiver, std::string msg) {
		return sendto(serverSocket, msg.c_str(), msg.length(), 0, (sockaddr*)&receiver, sizeof(receiver));
	}

	int Server::sendMessage(SOCKADDR_IN receiver, char* msg, int msgLength) {
		return sendto(serverSocket, msg, msgLength, 0, (sockaddr*)&receiver, sizeof(receiver));
	}

	void Server::clearMessageBuffer() {
		ZeroMemory(messageBuffer, MESSAGE_BUFFER_SIZE);
	}

	void Server::openToClientConnection() {
		int currentClientCount = 0;

		clientIPList.reserve(TARGET_CLIENT_COUNT);

		LOG_INFO("Searching for " + std::to_string(TARGET_CLIENT_COUNT) + " clients: ");
		while (currentClientCount < TARGET_CLIENT_COUNT) {
			SOCKADDR_IN clientAddr;
			int clientSize = sizeof(clientAddr);

			clearMessageBuffer();

			int bytesReceived = recvfrom(serverSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&clientAddr, &clientSize);

			if (bytesReceived > 0) {
				try {
					// Capture Message
					std::string receivedMsg = std::string(messageBuffer, messageBuffer + bytesReceived);

					if (receivedMsg == Messages::JOIN) {
						clientIPList.emplace_back(clientAddr);
						sendMessage(clientAddr, std::to_string(ClientNode::getLastNodeID()));
						LOG_INFO("    [" + std::to_string(currentClientCount) + "/" + std::to_string(TARGET_CLIENT_COUNT) + "] client found: " + clientIPList.at(currentClientCount).to_string());
						++currentClientCount;
					}
				}
				catch (std::invalid_argument& e) {}
				catch (std::out_of_range& e) {}
				catch (...) {}
			}

			Sleep(1);
		}
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
				sendMessage(clientIPList.at(i).getClient(), update, MSG_LENGTH);
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
					sendMessage(clientAddr, "");
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