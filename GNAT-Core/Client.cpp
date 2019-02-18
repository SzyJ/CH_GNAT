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

    bool Client::compareMessageCode(const std::string& message, const std::string& expectedMessage) {
        for (int i = 0; i < Messages::LENGTH; ++i) {
            if (message[i] != expectedMessage[i]) {
                return false;
            }
        }

        return true;
    }

	bool Client::sendToServer(std::string message, const char* onErrorMsg = nullptr) {
		int bytesSent = IP_Utils::sendMessage(clientSocket, serverAddress, message);
		if (bytesSent != message.length) {
			if (onErrorMsg != nullptr) {
				CLIENT_LOG_ERROR(onErrorMsg);
			} else {
				CLIENT_LOG_ERROR("Failed to send message to server: " + message);
			}
			return false;
		}

		return true;
	}

	std::string Client::listenForServerMessage(const int rejectByteLength = 0, const std::string expectedMsgCode = "", const int maxRetryCount = 1500) {
		int retryCount = 0;

		CLIENT_LOG_INFO("Waiting for Server Response...");

		std::string msg = "";

		// TODO: dont convert to string but let data be sent as bits in chars

		while (retryCount++ < maxRetryCount || maxRetryCount < 0) {
			SOCKADDR_IN serverAddr;
			int serverSize = sizeof(serverAddr);

			int bytesReceived = recvfrom(clientSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverSize);

			if (bytesReceived > rejectByteLength) {

				try {
					msg = std::string(messageBuffer, messageBuffer + bytesReceived);

					if (expectedMsgCode == "") {
						return msg;
					} else if (compareMessageCode(msg, expectedMsgCode)) {
						return msg.substr(Messages::LENGTH);
					} else {
						CLIENT_LOG_WARN("Received message from server with unexpected message code, IGNORING");
					}

				} catch (...) {
					CLIENT_LOG_ERROR("An exception occurred when reading server response");
					return NULL;
				}

			} else if (bytesReceived > 0) {
				CLIENT_LOG_WARN("Received message from server below threashold, IGNORING");
			}

			Sleep(1);
		}

		CLIENT_LOG_WARN("Failed to get response from server. Timed out.");

		return NULL;
	}

    bool Client::sendJoinRequest() {
        bool sendSuccessful = sendToServer(Messages::JOIN_REQ, "Failed to send join request");
        if (!sendSuccessful) {
            return false;
        }


		std::string serverMsg = listenForServerMessage(Messages::LENGTH, Messages::JOIN_ACC);
		// server response is rejected unless it is longer than Messages::LENGTH
		if (serverMsg == "" && std::isdigit(serverMsg[0])) {
			CLIENT_LOG_ERROR("Failed to receive join ack from server. Aborting connection");
			return false;
		}

		thisID = serverMsg[0];



		// TODO: convert this to use helper method.
        bool awaitingJoinAccept = true;
        while (awaitingJoinAccept) {
			SOCKADDR_IN serverAddr;
			int serverSize = sizeof(serverAddr);

            int bytesReceived = recvfrom(clientSocket, messageBuffer, MESSAGE_BUFFER_SIZE, 0, (sockaddr*)&serverAddr, &serverSize);

            if (bytesReceived >= Messages::LENGTH + PLAYER_COUNT) {
                try {
                    // Capture Message
                    std::string receivedMsg = std::string(messageBuffer, messageBuffer + bytesReceived);

                    if (compareMessageCode(receivedMsg, Messages::JOIN_ACC)) {
						awaitingJoinAccept = false;
						if (receivedMsg.length() > Messages::LENGTH) {
							// Get ID from server
							thisID = receivedMsg[Messages::LENGTH];

							CLIENT_LOG_INFO("Client ID set to: " + std::to_string(thisID));
							return true;
						} else {
							CLIENT_LOG_ERROR("Failed to get initial Value from Server. Aborting connection.");
							return false;
						}
						
                    }
                }
				catch (std::invalid_argument& e) { awaitingJoinAccept = false; }
                catch (std::out_of_range& e) { awaitingJoinAccept = false; }
                catch (...) { awaitingJoinAccept = false; }
            }

			Sleep(1);
        }

		// TODO: Get initial Value from server
		thisVal = &playerStates.at(thisID);
		// End Get init values

        return false;
    }

	char Client::checkForUserInput() inline {

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

		sendUpdates = std::thread(userInputLoop());
		recvUpdates = std::thread(stateUpdateLoop());

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