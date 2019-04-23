#pragma once
#include "pch.h"
#include "ErrorCodes.h"
#include "ConnectionClient.h"
#include "Messages.h"
#include "ServerConfigs.h"
#include "GameConfigs.h"
#include "PeerToPeerSessionConfigs.h"
#include "ClientNode.h"

ConnectionClient::ConnectionClient() {
	GNAT::GNAT_Log::init_connection();
}

ConnectionClient::~ConnectionClient() {
	closesocket(clientSocket);
	WSACleanup();
}

int ConnectionClient::initializeWinSock(const char* address, const USHORT port) {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		CONNECT_LOG_ERROR("Failed to initialise WinSock in connection client.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (clientSocket < 0) {
		CONNECT_LOG_ERROR("A socket could not be created. Aborting...");
		WSACleanup();
		return SOCKET_CREATION_FAIL;
	}

	// Define Server Info
	ZeroMemory(&hint, sizeof(hint));
	hint.sin_port = htons(port);
	hint.sin_family = AF_INET;
	inet_pton(AF_INET, address, &hint.sin_addr);

	listen(clientSocket, SOMAXCONN);

	return STARTUP_SUCCESSFUL;
}

int ConnectionClient::connectToServer() {
	if (connect(clientSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		CONNECT_LOG_ERROR("Failed to connect to the server, Aborting!");
		closesocket(clientSocket);
		WSACleanup();
		return FAILED_TO_CONNECT_TO_SERVER;
	}

	connectionEstablished = true;

	return CONNECTION_ESTABLISHED;
}

int ConnectionClient::sendJoinRequest(u_short udpPort) {
	std::string port = std::to_string(udpPort);
	const int PORT_STRING_LENGTH = port.length();

	CONNECT_LOG_INFO("UDP PORT: " + port);

	if (!connectionEstablished) {
		CONNECT_LOG_ERROR("Attempting to send message without establishing connection...");
		return NOT_CONNECTED_TO_SERVER;
	}

	const int RECEIVE_BUFFER_SIZE = 1024;
	char receiveBuffer[RECEIVE_BUFFER_SIZE];

	const int JOIN_MESSAGE_LENGTH = MESSAGE_LENGTH + PORT_STRING_LENGTH;
	char* joinMessageBuffer = new char[JOIN_MESSAGE_LENGTH];
	memcpy(joinMessageBuffer, Messages::JOIN_REQ, MESSAGE_LENGTH);
	memcpy(joinMessageBuffer + MESSAGE_LENGTH, port.c_str(), PORT_STRING_LENGTH);

	int bytesSent = send(clientSocket, joinMessageBuffer, JOIN_MESSAGE_LENGTH, 0);
	if (bytesSent < JOIN_MESSAGE_LENGTH) {
		CONNECT_LOG_ERROR("Failed to send Join Message to server...");
		return FAILED_TO_SEND_MESSAGE;
	}

	// Listen for server response
	int bytesReceived = 0;
	bool response = false;
	while (!response) {
		ZeroMemory(receiveBuffer, RECEIVE_BUFFER_SIZE);
		bytesReceived = recv(clientSocket, receiveBuffer, RECEIVE_BUFFER_SIZE, 0);

		if (bytesReceived > 0) {
			CONNECT_LOG_INFO("MSG [" + std::to_string(bytesReceived) + " bytes]: " + std::string(receiveBuffer, bytesReceived));
			response = true;
		} else {
			Sleep(5);
		}
	}

	if (!Messages::codesMatch(receiveBuffer, bytesReceived, Messages::JOIN_ACC)) {
		CONNECT_LOG_INFO("Unexpected message received");
		return UNEXPECTED_MESSAGE;
	}

	Messages::dataByte receivedID(receiveBuffer[MESSAGE_LENGTH]);
	clientID = receivedID.unsignedByte;

	CONNECT_LOG_INFO("Successfully joined lobby. ID: " + std::to_string(clientID));

	return (int) clientID;
}

int ConnectionClient::listenForPeerInfo() {
	if (clientID == 0) {
		CONNECT_LOG_WARN("This client's ID has not been initialised. All clients will be added to client list.");
	}

	int receivedClientCounter = 0;

	int bytesReceived = 0;
	const int RECEIVE_BUFFER_SIZE = 1024;
	char receiveBuffer[RECEIVE_BUFFER_SIZE];

	clientIPList = new std::vector<ClientNode*>;
	clientIPList->reserve(TARGET_PLAYER_COUNT - 1);

	while (receivedClientCounter < TARGET_PLAYER_COUNT) {
		ZeroMemory(receiveBuffer, RECEIVE_BUFFER_SIZE);
		bytesReceived = recv(clientSocket, receiveBuffer, RECEIVE_BUFFER_SIZE, 0);

		if (!Messages::codesMatch(receiveBuffer, bytesReceived, Messages::DEFINE)) {
			continue;
		}

		Messages::dataByte thisID(receiveBuffer[MESSAGE_LENGTH]);
		
		if (thisID.unsignedByte == clientID) {
			// (Possible future improvement)
			// This clinet's info is ignored.
			// Could check if correct here and send message if not.

			++receivedClientCounter;
			continue;
		}

		const int MESSAGE_WITH_ID_LENGTH = MESSAGE_LENGTH + ID_LENGTH;
		
		if (bytesReceived <= MESSAGE_WITH_ID_LENGTH) {
			CONNECT_LOG_ERROR("Invalid Message length received. Message: " + std::string(receiveBuffer, bytesReceived));
			continue;
		}

		const std::string ADDRESS_PORT_SEPERATOR(":");
		std::string addressString(receiveBuffer + MESSAGE_WITH_ID_LENGTH, bytesReceived - MESSAGE_WITH_ID_LENGTH);

		size_t seperatorPos = 0;
		if ((seperatorPos = addressString.find(ADDRESS_PORT_SEPERATOR)) == std::string::npos) {
			CONNECT_LOG_ERROR("Unrecognised string format received. Ignoring: " + addressString);
			continue;
		}

		USHORT port = atoi(addressString.substr(seperatorPos + ADDRESS_PORT_SEPERATOR.length(), addressString.length()).c_str());
		const char* address = addressString.substr(0, seperatorPos).c_str();


		clientIPList->emplace_back(new ClientNode(thisID.unsignedByte, (address == SESSION_HOST_TOKEN) ? SESSION_HOST_ADDRESS : address, port));
		CONNECT_LOG_INFO("Added node: [" + std::to_string((char)thisID.unsignedByte) + "] " + addressString);
		CONNECT_LOG_INFO("Clients:");
		for (ClientNode* node : *clientIPList) {
			CONNECT_LOG_INFO("    [" + std::to_string(node->getNodeID()) + "] " + node->to_string());
		}
		++receivedClientCounter;
	}

	return 0;
}

std::vector<ClientNode*>* ConnectionClient::getClientList() {
	if (clientIPList == nullptr) {
		CONNECT_LOG_ERROR("Client IP list has not been initialised. Returning null ptr");
		return nullptr;
	}

	return clientIPList;
}