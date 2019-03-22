#include "pch.h"
#include "GameClient.h"
#include "ErrorCodes.h"
#include <thread>

GameClient::GameClient() {

}

GameClient::~GameClient() {
	closesocket(clientSocket);
	WSACleanup();
}

void GameClient::ListenForUpdates() {
	const int BUFFER_SIZE = 1024;
	char msgBuffer[BUFFER_SIZE];

	while (true) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		int bytesReceived = recvfrom(clientSocket, msgBuffer, BUFFER_SIZE, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

		if (bytesReceived > 0) {
			// Update Values
		} else {
			CLIENT_LOG_ERROR("The connection to the server has been lost.");
		}

		Sleep(1);
	}
}

int GameClient::initializeWinsock() {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		CLIENT_LOG_ERROR("Failed to initialise WinSock in connection server.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (clientSocket < 0) {
		CLIENT_LOG_ERROR("A socket could not be created. Aborting...");
		WSACleanup();
		return SOCKET_CREATION_FAIL;
	}

	// Define Server Info
	ZeroMemory(&serverHint, sizeof(serverHint));
	serverHint.sin_port = htons(SERVER_PORT);
	serverHint.sin_family = AF_INET;
	inet_pton(AF_INET, SERVER_ADDR, &serverHint.sin_addr);

	// Define Client Info
	int clientSize = sizeof(clientHint);
	ZeroMemory(&clientHint, clientSize);
	clientHint.sin_port = 0; // Any Port
	clientHint.sin_family = AF_INET;
	clientHint.sin_addr.s_addr = INADDR_ANY;

	// Bind Socket
	if (bind(clientSocket, (LPSOCKADDR)&clientHint, clientSize) == SOCKET_ERROR) {
		CLIENT_LOG_ERROR("Failed to bind client socket. Aborting...");
		WSACleanup();
		return BINDING_SOCKET_FAIL;
	}

	// Set Socket Options
	int val = 64 * 1024;
	setsockopt(clientSocket, SOL_SOCKET, SO_SNDBUF, (char*)&val, sizeof(val));
	
	return STARTUP_SUCCESSFUL;
}

int GameClient::startClient() {
	SetConsoleTitleA("Client");

	std::thread listenThread(&ListenForUpdates);

	while (true) {
		// Listen for keyboard input

		// Make message with update value
		char* msg;
		int msgLen = sizeof(msg);

		// Send message to server
		sendto(clientSocket, msg, msgLen, 0, (sockaddr*)&serverHint, sizeof(serverHint));
		Sleep(500);
	}

}