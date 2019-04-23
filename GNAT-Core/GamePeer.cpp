#pragma once
#include "pch.h"
#include "GamePeer.h"
#include "ServerConfigs.h"
#include "Messages.h"
#include "ErrorCodes.h"

GamePeer::GamePeer() {
	SetConsoleTitleA("Peer");
}

GamePeer::~GamePeer() {
	closesocket(peerSocket);
	WSACleanup();
}

char GamePeer::checkForUserInput() {
	bool isConsoleWindowFocussed = (GetConsoleWindow() == GetForegroundWindow());
	if (!isConsoleWindowFocussed) {
		return NULL;
	}

	for (int i = 0; i < 10; ++i) {
		if (GetAsyncKeyState(0x30 + i)) {
			return '0' + i;
		}
	}

	return NULL;
}


bool GamePeer::sendToPeers(const char* message, const int messageLen, const char* onErrorMsg) {
	PEER_LOG_INFO("Sending message to Peers: " + std::string(message, messageLen));

	for (ClientNode* node : *peerIPList) {
		int bytesSent = IP_Utils::sendMessage(peerSocket, node->getClient(), message, messageLen);
		if (bytesSent != messageLen) {
			if (onErrorMsg != nullptr) {
				PEER_LOG_ERROR(onErrorMsg);
			} else {
				PEER_LOG_ERROR("Failed to send message to peer: " + node->to_string());
			}
		}
	}

	return true;
}

void GamePeer::ListenForUpdates() {
	const int BUFFER_SIZE = 1024;
	char msgBuffer[BUFFER_SIZE];

	while (threadsRunning) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		int bytesReceived = recvfrom(peerSocket, msgBuffer, BUFFER_SIZE, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

		if (bytesReceived > 0) {
			PEER_LOG_INFO("Message Received: " + std::string(msgBuffer, bytesReceived));

			if (bytesReceived >= MESSAGE_LENGTH + 2 &&
				Messages::codesMatch(msgBuffer, bytesReceived, Messages::UPDATE)) {
				Messages::dataByte idByte(msgBuffer[MESSAGE_LENGTH]);
				char val(msgBuffer[MESSAGE_LENGTH + 1]);

				for (ClientNode* client : *peerIPList) {
					if (client->getClient().sin_addr.S_un.S_addr == remoteAddr.sin_addr.S_un.S_addr
						&& client->getNodeID() == idByte.unsignedByte) {
						SERVER_LOG_INFO("Updating ID " + std::to_string((char)idByte.unsignedByte) + " value to: " + std::to_string(val));
						client->setUpdateValue(val);
						break;
					}
				}
			}
		}

		Sleep(1);
	}
}

void GamePeer::ListenForKeyboard() {
	// Listen for keyboard input and send update to server
	char validKeycodeUpdate = NULL;

	while (threadsRunning) {
		validKeycodeUpdate = checkForUserInput();

		if (validKeycodeUpdate == NULL ||
			validKeycodeUpdate == thisVal) {
			Sleep(1);
			continue;
		}

		const int thisMsgLength = MESSAGE_LENGTH + 1;
		char message[thisMsgLength];
		memcpy(message, Messages::UPDATE, MESSAGE_LENGTH);
		message[MESSAGE_LENGTH] = validKeycodeUpdate;

		sendToPeers(message, thisMsgLength);

		validKeycodeUpdate = NULL;

		Sleep(1);
	}
}

bool GamePeer::setClientList(std::vector<ClientNode*>* peerIPList) {
	bool overriden = this->peerIPList != NULL && peerIPList != NULL && this->peerIPList != peerIPList;
	this->peerIPList = peerIPList;
	return overriden;
}

int GamePeer::initializeWinSock() {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		PEER_LOG_ERROR("Failed to initialise WinSock in connection to game server.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	peerSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (peerSocket < 0) {
		PEER_LOG_ERROR("A socket could not be created. Aborting...");
		WSACleanup();
		return SOCKET_CREATION_FAIL;
	}

	// Define peer Info
	int peerSize = sizeof(peerHint);
	ZeroMemory(&peerHint, peerSize);
	peerHint.sin_port = 0; // Any Port
	peerHint.sin_family = AF_INET;
	peerHint.sin_addr.s_addr = INADDR_ANY;

	// Bind Socket
	if (bind(peerSocket, (LPSOCKADDR)&peerHint, peerSize) == SOCKET_ERROR) {
		PEER_LOG_ERROR("Failed to bind peer socket. Aborting...");
		WSACleanup();
		return BINDING_SOCKET_FAIL;
	}

	// Set Socket Options
	int val = 64 * 1024;
	setsockopt(peerSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));

	listen(peerSocket, SOMAXCONN);

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(peerSocket, (struct sockaddr *)&sin, &len) == -1) {
		return GETTING_PORT_FAILED;
	}

	USHORT port = ntohs(sin.sin_port);
	SetConsoleTitleA(("Peer[" + std::to_string(port) + "]").c_str());

	return port;
}

int GamePeer::startGame() {
	threadsRunning = true;

	std::thread updateThread([=] { ListenForKeyboard(); });
	std::thread listenThread([=] { ListenForUpdates(); });

	while (std::cin.get() != 27);

	threadsRunning = false;
	CLIENT_LOG_INFO("Waiting for threads to finish.");
	updateThread.join();
	listenThread.join();
	CLIENT_LOG_INFO("Threads successfully closed.");

	return 0;
}