#pragma once
#include "pch.h"
#include "ErrorCodes.h"
#include "ConnectionClient.h"
#include "Messages.h"
#include "ServerConfigs.h"

ConnectionClinet::ConnectionClinet() {
	GNAT::GNAT_Log::init_connection();
}

ConnectionClinet::~ConnectionClinet() {
	closesocket(clientSocket);
	WSACleanup();
}

int ConnectionClinet::initializeWinSock() {
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
	hint.sin_port = htons(PORT);
	hint.sin_family = AF_INET;
	inet_pton(AF_INET, ADDRESS, &hint.sin_addr);

	listen(clientSocket, SOMAXCONN);

	return STARTUP_SUCCESSFUL;
}

int ConnectionClinet::connectToServer() {
	if (connect(clientSocket, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		CONNECT_LOG_ERROR("Failed to connect to the server, Aborting!");
		closesocket(clientSocket);
		WSACleanup();
		return FAILED_TO_CONNECT_TO_SERVER;
	}

	connectionEstablished = true;

	return CONNECTION_ESTABLISHED;
}

int ConnectionClinet::sendJoinRequest(u_short udpPort) {
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

	Messages::dataByte clientID(receiveBuffer[MESSAGE_LENGTH]);
	CONNECT_LOG_INFO("Successfully joined lobby. ID: " + std::to_string(clientID.unsignedByte));
	return (int) clientID.unsignedByte;
}