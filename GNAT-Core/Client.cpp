#include "pch.h"
#include "Client.h"
#include "IPUtils.h"
#include "Messages.h"

namespace GNAT {
    const std::string Client::SERVER_ADDRESS = "127.0.0.1";
    const USHORT Client::SERVER_PORT = 54000;

    Client::Client() {
		GNAT_Log::init_client();

        // Startup WinSock
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            errorFlag = WINSOCK_STARTUP_FAIL;
            CLIENT_LOG_ERROR("Failed to startup Winsock starting client");
            return;
        }
        CLIENT_LOG_INFO("WinSock startup success");

        // Set connection as UDP
        clientSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        // Define server address
        serverAddress.sin_port = SERVER_PORT;
        serverAddress.sin_family = AF_INET;
        inet_pton(AF_INET, SERVER_ADDRESS.c_str(), &serverAddress.sin_addr);

        // Define client socket
        clientAddr.sin_port = 0; // Choose any port
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

	int Client::establishTCPConnection() {

		return 0;
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

			//try {
				int bytesReceived = recvfrom(clientSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverSize);
				if (bytesReceived > 0) {
					CLIENT_LOG_INFO("Message recieved from server: " + std::string(messageBuffer, bytesReceived));

					return bytesReceived;
				}
			/*} catch (...) {
				CLIENT_LOG_ERROR("An exception occurred when reading server response");
				return EXEPTION_DURING_MSG_RECEIVE;
			}*/

			Sleep(1);
		}
		
		return NO_MESSAGE_TO_RECEIVE;
	}

	int Client::listenForClientID() {
		bool awaitingJoinAccept = true;
		
		while (awaitingJoinAccept) {
			int serverMsgLength;

			try {
				serverMsgLength = listenForServerMessage();
			}
			catch (...) {
				CLIENT_LOG_ERROR("Exception occurred when receiving data from server. Error code: " + std::to_string(WSAGetLastError()));
				Sleep(100);
				continue;
			}

			if (serverMsgLength > 0) {
				if (!Messages::codesMatch(messageBuffer, serverMsgLength, Messages::JOIN_ACC)) {
					CLIENT_LOG_ERROR("Failed to receive join ack from server. Retrying.... Server Msg: " + std::string(messageBuffer, serverMsgLength));
					Sleep(100);
					continue;
				}
				else if (serverMsgLength == MESSAGE_LENGTH) {
					CLIENT_LOG_ERROR("Server join Acknowlegement did not contain this client's ID. Aborting connection. Server Msg: " + std::string(messageBuffer, serverMsgLength));
					Sleep(100);
					continue;
				}

				// Assuming that protocol only alows 1 byte for ID
				Messages::dataByte receivedID(messageBuffer[MESSAGE_LENGTH]);

				return (int) receivedID.unsignedByte;

				awaitingJoinAccept = false;
			}

			Sleep(1);
		}

		return 0;
	}

	int Client::listenForServerInit() {
		const int RETRY_COUNT = 3000;

		// TODO: Maybe use a loop?
		int bytesReceived = listenForServerMessage(RETRY_COUNT);

		if (bytesReceived < MESSAGE_LENGTH) {
			// Incorrect message Received
			CLIENT_LOG_WARN("Incorrect message length received from server. Length: " + bytesReceived);
			return -1; // TODO add error for failure to init
		}

		const int CLIENT_COUNT = (bytesReceived - MESSAGE_LENGTH) / 2;

		for (int i = MESSAGE_LENGTH; i < CLIENT_COUNT; i += 2) {
			Messages::dataByte thisID(messageBuffer[i]);
			playerStates.at(thisID.unsignedByte) = messageBuffer[i + 1];
		}

		return CLIENT_COUNT;
	}


    bool Client::sendJoinRequest() {
        bool sendSuccessful = sendToServer(Messages::JOIN_REQ, MESSAGE_LENGTH, "Failed to send join request");
        if (!sendSuccessful) {
            return false;
        }
		
		int receivedID = listenForClientID();
		if (receivedID < 0) {
			CLIENT_LOG_ERROR("An error occurred while getting client ID from server. Aborting connection. Error: " + std::to_string(receivedID));
			return false;
		}
		if (receivedID > UCHAR_MAX) {
			CLIENT_LOG_ERROR("An invalid ID has been received from the server. Aborting connection. ID: " + std::to_string(receivedID));
			return false;
		}
		thisID = (byte) receivedID;

		bool initSuccessful = listenForServerInit();
		if (!initSuccessful) {
			CLIENT_LOG_ERROR("Failed to initialise values. Either server returned invalid data or initialisation failed.");
			return false;
		}

        return true;
    }

	char Client::checkForUserInput() {

		for (int i = 0; i < 10; ++i) {
			if (GetAsyncKeyState(0x30 + i)) {
				return '0' + i;
			}
		}

		return NULL;
	}

	void Client::userInputLoop() {
		// Listen for keyboard input and send update to server
		char validKeycodeUpdate = NULL;

		while (true) {
			validKeycodeUpdate = checkForUserInput();

			if (validKeycodeUpdate == NULL ||
				validKeycodeUpdate == *thisVal) {
				Sleep(1);
				continue;
			}

			const int thisMsgLength = MESSAGE_LENGTH + 1;
			char message[thisMsgLength];
			memcpy(message, Messages::UPDATE, MESSAGE_LENGTH);
			message[MESSAGE_LENGTH] = validKeycodeUpdate;
			
			sendToServer(message, thisMsgLength);

			validKeycodeUpdate = NULL;

			Sleep(1);
		}
	}

    void Client::stateUpdateLoop() {
		while (true) {
			int bytesReceived = listenForServerMessage();

			if (bytesReceived < MESSAGE_LENGTH || (bytesReceived - MESSAGE_LENGTH) % 2 != 0) {
				// Wrong Message Length received
				CLIENT_LOG_WARN("Incorrect message length received from server. Length: " + bytesReceived);

				Sleep(1);
				continue;
			}

			int updatesReceived = (bytesReceived - MESSAGE_LENGTH) / 2;

			for (int i = MESSAGE_LENGTH; i < updatesReceived; i += 2) {
				Messages::dataByte thisID(messageBuffer[i]);
				playerStates.at(thisID.unsignedByte) = messageBuffer[i + 1];
			}

			Sleep(1);
		}
    }

	bool Client::startListen() {
		if (threadsListening) {
			// TODO: Log info
			return false;
		}
		threadsListening = true;

		//sendUpdates = std::thread(userInputLoop());
		//recvUpdates = std::thread(stateUpdateLoop());

		return true;
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