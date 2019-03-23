#include "pch.h"
#include "GameServer.h"
#include "ErrorCodes.h"
#include "Messages.h"
#include <thread>
#include "ServerConfigs.h"

GameServer::GameServer() {
	SetConsoleTitleA(("Server [" + std::to_string(PORT) + "]").c_str());
}

GameServer::~GameServer() {
	closesocket(serverSocket);
	WSACleanup();
}

void GameServer::startStateBroadcast() {
	const int MAX_MSG_LENGTH = MESSAGE_LENGTH + (clientIPList->size() * 2);
	char* updateMsg = new char[MAX_MSG_LENGTH];

	while (true) {
		int clientCount = clientIPList->size();
		int thisMsgLength = MESSAGE_LENGTH + (clientCount * 2);
		strcpy_s(updateMsg, MESSAGE_LENGTH, Messages::CURRENT_STATE);

		int clinetStepper = 0;
		for (int i = MESSAGE_LENGTH; clinetStepper < clientCount; i += 2, ++clinetStepper) {
			ClientNode* thisClient = clientIPList->at(clinetStepper);

			Messages::dataByte thisID(thisClient->getNodeID());
			char thisVal = thisClient->getUpdateValue();

			updateMsg[i] = thisID.signedByte;
			updateMsg[i + 1] = thisVal;
		}

		// Broadcast Message
		for (int i = 0; i < clientCount; ++i) {
			IP_Utils::sendMessage(serverSocket, clientIPList->at(i)->getClient(), updateMsg, thisMsgLength);
		}

		Sleep(SEND_DELAY);
	}
}

bool GameServer::setClientList(std::vector<ClientNode*>* clientIPList) {
	bool overriden = this->clientIPList != NULL;
	this->clientIPList = clientIPList;
	return overriden;
}

int GameServer::initializeWinSock() {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		SERVER_LOG_ERROR("Failed to initialise WinSock in game server.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	serverSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (serverSocket < 0) {
		SERVER_LOG_ERROR("A socket could not be created. Aborting...");
		WSACleanup();
		return SOCKET_CREATION_FAIL;
	}

	// Define Server Info
	const int SERVER_HINT_SIZE = sizeof(serverHint);
	ZeroMemory(&serverHint, SERVER_HINT_SIZE);
	serverHint.sin_port = htons(PORT);
	serverHint.sin_family = AF_INET;
	serverHint.sin_addr.s_addr = htonl(INADDR_ANY);

	// Bind Socket
	if (bind(serverSocket, (LPSOCKADDR)&serverHint, SERVER_HINT_SIZE) == SOCKET_ERROR) {
		SERVER_LOG_ERROR("Failed to bind server socket. Aborting...");
		WSACleanup();
		return BINDING_SOCKET_FAIL;
	}

	// Set Socket Options
	int val = 64 * 1024;
	setsockopt(serverSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));

	listen(serverSocket, SOMAXCONN);

	return STARTUP_SUCCESSFUL;
}

int GameServer::startServer() {
	//std::thread stateUpdate(&startStateBroadcast);

	const int BUFFER_LENGTH = 1024;
	char buffer[BUFFER_LENGTH];

	while (true) {
		ZeroMemory(buffer, BUFFER_LENGTH);
		SOCKADDR_IN clientAddr;
		int clientSize = sizeof(clientAddr);

		int bytesReceived = recvfrom(serverSocket, buffer, BUFFER_LENGTH, 0, (sockaddr*)&clientAddr, &clientSize);

		if (bytesReceived > 0) {
			SERVER_LOG_INFO("Message Received: " + std::string(buffer, bytesReceived));
			
			if (bytesReceived >= MESSAGE_LENGTH + 2 &&
				Messages::codesMatch(buffer, bytesReceived, Messages::UPDATE)) {
				Messages::dataByte idByte(buffer[MESSAGE_LENGTH]);
				char val(buffer[MESSAGE_LENGTH + 1]);

				for (ClientNode* client : *clientIPList) {
					if (client->getClient().sin_addr.S_un.S_addr == clientAddr.sin_addr.S_un.S_addr
						&& client->getNodeID() == idByte.unsignedByte) {
						client->setUpdateValue(val);
						break;
					}
				}
			}
		}

		Sleep(1);
	}

	return 0;
}