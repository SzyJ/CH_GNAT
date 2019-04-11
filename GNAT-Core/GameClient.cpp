#include "pch.h"
#include "GameClient.h"
#include "ErrorCodes.h"
#include "IPUtils.h"
#include "Messages.h"
#include <thread>
#include "ServerConfigs.h"

GameClient::GameClient() {
	SetConsoleTitleA("Client");
}

GameClient::~GameClient() {
	closesocket(clientSocket);
	WSACleanup();
}

char GameClient::checkForUserInput() {
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


bool GameClient::sendToServer(const char* message, const int messageLen, const char* onErrorMsg) {
	int bytesSent = IP_Utils::sendMessage(clientSocket, serverHint, message, messageLen);
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


void GameClient::ListenForUpdates() {
	const int BUFFER_SIZE = 1024;
	char msgBuffer[BUFFER_SIZE];

	while (threadsRunning) {
		SOCKADDR_IN remoteAddr;
		int	remoteAddrLen = sizeof(remoteAddr);

		int bytesReceived = recvfrom(clientSocket, msgBuffer, BUFFER_SIZE, 0, (sockaddr*)&remoteAddr, &remoteAddrLen);

		if (bytesReceived > 0) {
			// Update Values
			CLIENT_LOG_INFO("Received message: " + std::string(msgBuffer, bytesReceived));
		} else {
			CLIENT_LOG_ERROR("The connection to the server has been lost.");
		}

		Sleep(1);
	}
}

void GameClient::ListenForKeyboard() {
	// Listen for keyboard input and send update to server
	char validKeycodeUpdate = NULL;

	using namespace std::literals::chrono_literals;

	while (threadsRunning) {
		validKeycodeUpdate = checkForUserInput();

		if (validKeycodeUpdate == NULL) {
			std::this_thread::sleep_for(0.5s);
			continue;
		}

		const int thisMsgLength = MESSAGE_LENGTH + 2;
		char message[thisMsgLength];
		memcpy(message, Messages::UPDATE, MESSAGE_LENGTH);
		message[MESSAGE_LENGTH] = Messages::dataByte(thisID).signedByte;
		message[MESSAGE_LENGTH + 1] = validKeycodeUpdate;

		sendToServer(message, thisMsgLength);

		validKeycodeUpdate = NULL;

		std::this_thread::sleep_for(1s);
	}
}

int GameClient::initializeWinSock() {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		CLIENT_LOG_ERROR("Failed to initialise WinSock in connection to game server.");
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
	inet_pton(AF_INET, SERVER_ADDRESS, &serverHint.sin_addr);

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
	
	listen(clientSocket, SOMAXCONN);

	struct sockaddr_in sin;
	socklen_t len = sizeof(sin);
	if (getsockname(clientSocket, (struct sockaddr *)&sin, &len) == -1) {
		return GETTING_PORT_FAILED;
	}

	USHORT portInHostByteOrder = ntohs(sin.sin_port);

	SetConsoleTitleA(("Client[" + std::to_string(portInHostByteOrder) + "]").c_str());

	return portInHostByteOrder;
}

int GameClient::startClient() {
	if (thisID == 0) {
		CLIENT_LOG_ERROR("Invalid client ID provided. Aborting...");
		return INVALID_CLIENT_ID;
	}

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


byte GameClient::getID() {
	return thisID;
}

void GameClient::setID(byte newID) {
	thisID = newID;
}