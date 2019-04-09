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

std::string NormalizedIPString(SOCKADDR_IN addr) {
	char host[16];
	ZeroMemory(host, 16);
	inet_ntop(AF_INET, &addr.sin_addr, host, 16);

	USHORT port = ntohs(addr.sin_port);

	int realLen = 0;
	for (int i = 0; i < 16; i++) {
		if (host[i] == '\00') {
			break;
		}
		realLen++;
	}

	std::string res(host, realLen);
	res += ":" + std::to_string(port);

	return res;
}

void GameServer::startStateBroadcast() {
	const int MAX_MSG_LENGTH = MESSAGE_LENGTH + (clientIPList->size() * 2);
	char* updateMsg = new char[MAX_MSG_LENGTH];
		 
	while (threadsRunning) {
		int clientCount = clientIPList->size();
		int thisMsgLength = MESSAGE_LENGTH + (clientCount * 2);
		memcpy(updateMsg, Messages::CURRENT_STATE, MESSAGE_LENGTH);

		int clinetStepper = 0;
		for (int i = MESSAGE_LENGTH; clinetStepper < clientCount; i += 2, ++clinetStepper) {
			ClientNode* thisClient = clientIPList->at(clinetStepper);

			Messages::dataByte thisID(thisClient->getNodeID());
			char thisVal = thisClient->getUpdateValue();

			updateMsg[i] = thisID.signedByte;
			updateMsg[i + 1] = thisVal;
		}

		// Broadcast Message
		SERVER_LOG_INFO("Sending Message: " + std::string(updateMsg, thisMsgLength));

		for (int i = 0; i < clientCount; ++i) {
			SERVER_LOG_INFO("Sent-To: " + NormalizedIPString(clientIPList->at(i)->getClient()));
			IP_Utils::sendMessage(serverSocket, clientIPList->at(i)->getClient(), updateMsg, thisMsgLength);
		}

		Sleep(SEND_DELAY);
	}
}

void GameServer::startUpdateListen() {
	const int BUFFER_LENGTH = 1024;
	char buffer[BUFFER_LENGTH];

	while (threadsRunning) {
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
						SERVER_LOG_INFO("Updating ID " + std::to_string((char) idByte.unsignedByte) + " value to: " + std::to_string(val));
						client->setUpdateValue(val);
						break;
					}
				}
			}
		}

		Sleep(1);
	}
}

bool GameServer::setClientList(std::vector<ClientNode*>* clientIPList) {
	bool overriden = this->clientIPList != NULL && clientIPList != NULL && this->clientIPList != clientIPList;
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
	threadsRunning = true;

	std::thread updateListen([=] { startUpdateListen(); });
	std::thread stateBroadcast([=] { startStateBroadcast(); });

	while (std::cin.get() != 27);

	threadsRunning = false;
	CLIENT_LOG_INFO("Waiting for threads to finish.");
	updateListen.join();
	stateBroadcast.join();
	CLIENT_LOG_INFO("Threads successfully closed.");

	return 0;
}