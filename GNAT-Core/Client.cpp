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

    bool Client::sendJoinRequest() {
        int bytesSent = IP_Utils::sendMessage(clientSocket, serverAddress, Messages::JOIN_REQ);
        if (bytesSent != Messages::LENGTH) {
            CLIENT_LOG_ERROR("Failed to send join request");
            return false;
        }

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
							thisVal = receivedMsg[Messages::LENGTH];
							CLIENT_LOG_INFO("Client initialized to: " + thisVal);
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
        }

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

		while (true) {

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
		sendUpdates = std::thread(userInputLoop());
		recvUpdates = std::thread(stateUpdateLoop());

		return false;
	}

    const int Client::getErrorCode() const {
        return errorFlag;
    }
}