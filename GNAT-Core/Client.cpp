#include "pch.h"
#include "Client.h"
#include "IPUtils.h"
#include "Messages.h"

namespace GNAT {
    const std::string Client::SERVER_ADDRESS = "127.0.0.1";
    const USHORT Client::SERVER_PORT = 54000;

    Client::Client() {
        // Startup WinSock
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            errorFlag = WINSOCK_STARTUP_FAIL;
            CLIENT_LOG_ERROR("Failed to startup Winsock starting client");
            return;
        }
        CLIENT_LOG_INFO("WinSock startup success");

        // Set connection as UDP
        clientSocket = socket(AF_INET, SOCK_DGRAM, 0);

        // Define server address
        serverAddress.sin_port = SERVER_PORT;
        serverAddress.sin_family = AF_INET;
        inet_pton(AF_INET, SERVER_ADDRESS.c_str(), &serverAddress.sin_addr);

        // Define client socket
        SOCKADDR_IN clientAddr;
        clientAddr.sin_port = 0;
        clientAddr.sin_family = AF_INET;
        clientAddr.sin_addr.s_addr = INADDR_ANY;

        if (bind(clientSocket, (LPSOCKADDR)&clientAddr, sizeof(clientAddr)) == SOCKET_ERROR) {
            errorFlag = BINDING_SOCKET_FAIL;
            CLIENT_LOG_ERROR("Failed to bind socket with client");
            return;
        }
        CLIENT_LOG_INFO("Successfully binded client to socket");

        int val = 64 * 1024;
        setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
        setsockopt(clientSocket, SOL_SOCKET, SO_RCVBUF, (char*)&val, sizeof(val));

    }

    Client::~Client() {
        CLIENT_LOG_INFO("Closing client connections and cleaning up winsock...");
        closesocket(clientSocket);
        WSACleanup();
        CLIENT_LOG_INFO("Winsock cleanup done. Client connections shutdown");
    }

	bool Client::sendToServer(const char* message, const int messageLen, const char* onErrorMsg) {
		int bytesSent = IP_Utils::sendMessage(clientSocket, serverAddress, message, messageLen);
		if (bytesSent != messageLen) {
			if (onErrorMsg != nullptr) {
				CLIENT_LOG_ERROR(onErrorMsg);
			} else {
				CLIENT_LOG_ERROR("Failed to send message to server: " + std::string(message, messageLen));
			}
			return false;
		}

		CLIENT_LOG_INFO("Message sent to server: " + std::string(message, messageLen));

		return true;
	}

	int Client::listenForServerMessage(const int maxRetryCount = 1500) {
		int retryCount = 0;

		CLIENT_LOG_INFO("Waiting for Server Response...");

		while (retryCount++ < maxRetryCount || maxRetryCount < 0) {
			SOCKADDR_IN serverAddr;
			int serverSize = sizeof(serverAddr);

			try {
				int bytesReceived = recvfrom(clientSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverSize);

				return bytesReceived;
			} catch (...) {
				CLIENT_LOG_ERROR("An exception occurred when reading server response");
				return EXEPTION_DURING_MSG_RECEIVE;
			}

			Sleep(1);
		}

		return NO_MESSAGE_TO_RECEIVE;
	}

    bool Client::sendJoinRequest() {
        bool sendSuccessful = sendToServer(Messages::JOIN_REQ, MESSAGE_LENGTH, "Failed to send join request");
        if (!sendSuccessful) {
            return false;
        }


		int serverMsgLength = listenForServerMessage();

		if (!Messages::codesMatch(messageBuffer, serverMsgLength, Messages::JOIN_ACC)) {
			CLIENT_LOG_ERROR("Failed to receive join ack from server. Aborting connection. Server Msg: " + std::string(messageBuffer, serverMsgLength));
			return false;
		} else if (serverMsgLength == MESSAGE_LENGTH) {
			CLIENT_LOG_ERROR("Server join Acknowlegement did not contain this client's ID. Aborting connection. Server Msg: " + std::string(messageBuffer, serverMsgLength));
			return false;
		}

		// Assuming that protocol only alows 1 byte for ID
		thisID = messageBuffer[MESSAGE_LENGTH];

        bool awaitingJoinAccept = true;
        while (awaitingJoinAccept) {
			SOCKADDR_IN serverAddr;
			int serverSize = sizeof(serverAddr);

			int bytesReceived = 0;
			try {
				bytesReceived = recvfrom(clientSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverSize);
			} catch (...) {
				CLIENT_LOG_ERROR("Exception occurred when receiving data from server. Error code: " + std::to_string(WSAGetLastError()));
				continue;
			}

			if (Messages::codesMatch(messageBuffer, bytesReceived, Messages::JOIN_ACC)) {
				if (bytesReceived < MESSAGE_LENGTH + 1) {
					CLIENT_LOG_ERROR("Failed to get initial Value from Server. Aborting connection.");
					return false;
				}
				awaitingJoinAccept = false;

				Messages::dataByte data(messageBuffer[MESSAGE_LENGTH]);
				thisID = data.unsignedByte;

				CLIENT_LOG_INFO("Client ID set to: " + std::to_string(thisID));
				return true;
			}

			Sleep(1);
        }

		// TODO: Get initial Value from server

        return false;
    }

	char Client::checkForUserInput() {

		for (int i = 0; i < 10; ++i) {
			if (GetAsyncKeyState(0x30 + i)) {
				return '0' + i;
			}
		}

		return NULL;
	}

	bool Client::userInputLoop() {
		// Listen for keyboard input and send update to server

		char validKeycodeUpdate = NULL;

		while (true) {
			validKeycodeUpdate = checkForUserInput();

			if (validKeycodeUpdate == NULL ||
				validKeycodeUpdate == *thisVal) {
				Sleep(1);
				continue;
			}

			// Send Update to server
		}


		return false;
	}

    bool Client::stateUpdateLoop() {
		const int serverUpdateBufferLength = 1024;
		char serverUpdateBuffer[serverUpdateBufferLength];

		while (true) {
			SOCKADDR_IN serverAddr;
			int	serverAddrLen = sizeof(serverAddr);

			ZeroMemory(serverUpdateBuffer, serverUpdateBufferLength);

			int iResult = recvfrom(clientSocket, serverUpdateBuffer, serverUpdateBufferLength, 0, (sockaddr*)&serverAddr, &serverAddrLen);

			if (iResult > 0) {
				// Update Received
			} else {
				// connection failed. Exit?
			}
		}

        return false;
    }

	bool Client::startListen() {
		if (threadsListening) {
			// Log info
			return false;
		}
		threadsListening = true;

		//sendUpdates = std::thread(userInputLoop());
		//recvUpdates = std::thread(stateUpdateLoop());

		return false;
	}

	bool Client::stopListen() {
		if (!threadsListening) {
			// Log info
			return false;
		}

		threadsListening = false;

		// Stop Threads somehow

		return true;
	}

    const int Client::getErrorCode() const {
        return errorFlag;
    }
}