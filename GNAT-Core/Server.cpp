#include "pch.h"
#include "Server.h"
#include "ServerErrors.h"
#include "IPUtils.h"
namespace GNAT {
	Server::Server() {
		SetConsoleTitleA(("Server [" + std::to_string(PORT) + "]").c_str());

		WSADATA wsaData;
		int wsaStartupCode = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (wsaStartupCode != 0) {
			errorFlag = WINSOCK_STARTUP_FAIL;
			return;
		}

		SOCKADDR_IN sockAddr;
		sockAddr.sin_port = htons(PORT);
		sockAddr.sin_family = AF_INET;
		sockAddr.sin_addr.s_addr = htonl(INADDR_ANY);

		serverSocket = socket(AF_INET, SOCK_DGRAM, 0);

		if (bind(serverSocket, (LPSOCKADDR)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR) {
			errorFlag = BINDING_SOCKET_FAIL;
			return;
		}

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

	void Server::clearMessageBuffer() {
		ZeroMemory(messageBuffer, MESSAGE_BUFFER_SIZE);
	}


	const int Server::getErrorCode() const {
		return errorFlag;
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
}