#include "pch.h"
#include "ErrorCodes.h"
#include "ConnectionServer.h"
#include "Messages.h"
#include "ServerConfigs.h"
#include "GameConfigs.h"

ConnectionServer::ConnectionServer() {
	clientIPList = new std::vector<ClientNode*>();
	clientIPList->reserve(TARGET_PLAYER_COUNT); // TODO: Implement some state holding for configurable client count
}

std::string NormalizedIPStringTest(SOCKADDR_IN addr) {
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


int ConnectionServer::initializeWinSock() {
	// Init WinSock
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(LOWVERSION, HIGHVERSION);
	if (WSAStartup(DllVersion, &wsaData) != 0) {
		SERVER_LOG_ERROR("Failed to initialise WinSock in connection server.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock < 0) {
		SERVER_LOG_ERROR("A socket could not be created. Aborting...");
		return SOCKET_CREATION_FAIL;
	}

	// Define Server Info
	ZeroMemory(&hint, sizeof(hint));
	hint.sin_port = htons(PORT);
	hint.sin_family = AF_INET;
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	// Bind
	if (bind(listenSock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		SERVER_LOG_ERROR("Failed to bind socket. Aborting...");
		return BINDING_SOCKET_FAIL;
	}

	listen(listenSock, SOMAXCONN);

	return STARTUP_SUCCESSFUL;
}

int ConnectionServer::establishTCPConnection() {
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listenSock, &master);

	std::map<SOCKET, SOCKADDR_IN> socketMap;

	int clientCount = 0;

	while(clientCount < TARGET_PLAYER_COUNT) {
		fd_set copy = master;
		int socketCount = select(0, &copy, nullptr, nullptr, nullptr);

		for (int i = 0; i < socketCount; ++i) {
			SOCKET thisSock = copy.fd_array[i];
			
			if (thisSock == listenSock) {
				// Accept new connection
				SOCKADDR_IN clientInfo;
				int clientInfoSize = sizeof(clientInfo);

				SOCKET client = accept(listenSock, (sockaddr*)&clientInfo, &clientInfoSize);

				FD_SET(client, &master);

				socketMap.insert(std::pair<SOCKET, SOCKADDR_IN>(client, clientInfo));

				// Send msg on join?
				SERVER_LOG_INFO("Clinet Has Connected! Size: " + std::to_string(socketMap.size()));
				

				std::map<SOCKET, SOCKADDR_IN>::iterator it;

				for (it = socketMap.begin(); it != socketMap.end(); it++)
				{
					SERVER_LOG_INFO("Item: " + NormalizedIPStringTest(it->second));
				}

			} else {
				// Accept new Message
				const int BUFFER_SIZE = 1024;
				char msgBuffer[BUFFER_SIZE];

				int msgLength = recv(thisSock, msgBuffer, BUFFER_SIZE, 0);
				if (msgLength <= 0) {
					SERVER_LOG_INFO("Removing socket.");
					closesocket(thisSock);
					FD_CLR(thisSock, &master);
					continue;
				} else if (msgLength < MESSAGE_LENGTH) {
					continue;
				}

				SERVER_LOG_INFO("Received Msg [" + std::to_string(msgLength) + " bytes]: " + std::string(msgBuffer, msgLength));

				if (Messages::codesMatch(msgBuffer, msgLength, Messages::JOIN_REQ)) {
					
					std::string udpPortString(msgBuffer + MESSAGE_LENGTH, msgLength - MESSAGE_LENGTH);
					USHORT udpPort = atoi(udpPortString.c_str());

					// Construct Message
					const int ACK_MSG_LEN = MESSAGE_LENGTH + ID_LENGTH;
					char joinAckMsg[ACK_MSG_LEN];
					memcpy(joinAckMsg, Messages::JOIN_ACC, MESSAGE_LENGTH);
					Messages::dataByte thisClientsID(ClientNode::getNextNodeID());
					joinAckMsg[MESSAGE_LENGTH] = thisClientsID.signedByte;

					SERVER_LOG_INFO("Sending Ack Message");
					int bytesSent = send(thisSock, joinAckMsg, ACK_MSG_LEN, 0);
					SERVER_LOG_INFO("Sent " + std::to_string(bytesSent) + " bytes.");
					if (bytesSent != ACK_MSG_LEN) {
						SERVER_LOG_ERROR("Failed to send the Join Ack Message. The cient has not been added.");
						continue;
					}

					if (socketMap.find(thisSock) != socketMap.end()) {
						SERVER_LOG_INFO("Adding Client to list: " + NormalizedIPStringTest(socketMap[thisSock]));
						socketMap[thisSock].sin_port = udpPort;
						ClientNode* node = new ClientNode(socketMap[thisSock]);
						node->setUpdateValue('0');
						clientIPList->emplace_back(node);
						++clientCount;
					} else {
						SERVER_LOG_INFO("Could not find clinet info, could not add client.");
						// At this point the client doesnt know yet that they have not been added.
					}

				} else if (Messages::codesMatch(msgBuffer, msgLength, Messages::EXIT)) {
					SERVER_LOG_INFO("Exit Message Received");
					// Not implemented
				}
			}
		}
	}

	return 0;
}

int ConnectionServer::broadcastClientState() {
	const int CLIENT_COUNT = clientIPList->size();
	if (CLIENT_COUNT == 0) {
		return -1;
	}

	return 0;
}

std::vector<ClientNode*>* ConnectionServer::getClientList() {
	return clientIPList;
}