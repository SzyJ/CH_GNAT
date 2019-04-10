#include "pch.h"
#include "ErrorCodes.h"
#include "ConnectionServer.h"
#include "Messages.h"
#include "ServerConfigs.h"
#include "GameConfigs.h"

ConnectionServer::ConnectionServer() {
	GNAT::GNAT_Log::init_connection();
}

std::string ConnectionServer::getAddressAsString(SOCKADDR_IN addr) {
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
		CONNECT_LOG_ERROR("Failed to initialise WinSock in connection server.");
		return WINSOCK_STARTUP_FAIL;
	}

	// Create Socket
	listenSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (listenSock < 0) {
		CONNECT_LOG_ERROR("A socket could not be created. Aborting...");
		return SOCKET_CREATION_FAIL;
	}

	// Define Server Info
	ZeroMemory(&hint, sizeof(hint));
	hint.sin_port = htons(PORT);
	hint.sin_family = AF_INET;
	hint.sin_addr.S_un.S_addr = INADDR_ANY;

	// Bind
	if (bind(listenSock, (sockaddr*)&hint, sizeof(hint)) == SOCKET_ERROR) {
		CONNECT_LOG_ERROR("Failed to bind socket. Aborting...");
		return BINDING_SOCKET_FAIL;
	}

	listen(listenSock, SOMAXCONN);

	return STARTUP_SUCCESSFUL;
}

int ConnectionServer::addLocalhostAsClientOnPort(USHORT udpPort) {

	// TODO: Change localhost to addres of this instance
	thisClient = new ClientNode(0, LOCALHOST, udpPort);
	return 0;
}

int ConnectionServer::establishTCPConnection() {
	fd_set master;
	FD_ZERO(&master);
	FD_SET(listenSock, &master);

	int clientCount = socketMap.size();

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

				ClientNode* newNode = new ClientNode(clientInfo);
				newNode->setUpdateValue('0');
				socketMap.insert(std::pair<SOCKET, ClientNode*>(client, newNode));

				std::map<SOCKET, ClientNode*>::iterator it;

				for (it = socketMap.begin(); it != socketMap.end(); it++)
				{
					CONNECT_LOG_INFO("Item [id=" + std::to_string(Messages::dataByte(it->second->getNodeID()).signedByte) +  "]: " + it->second->to_string());
				}

			} else {
				// Accept new Message
				const int BUFFER_SIZE = 1024;
				char msgBuffer[BUFFER_SIZE];

				int msgLength = recv(thisSock, msgBuffer, BUFFER_SIZE, 0);
				
				if (msgLength < MESSAGE_LENGTH) {
					continue;
				}
				

				CONNECT_LOG_INFO("Received Msg [" + std::to_string(msgLength) + " bytes]: " + std::string(msgBuffer, msgLength));

				if (Messages::codesMatch(msgBuffer, msgLength, Messages::JOIN_REQ)) {
					if (socketMap.find(thisSock) == socketMap.end()) {
						CONNECT_LOG_ERROR("Could not find clinet info, could not add client.");
						continue;
					}

					std::string udpPortString(msgBuffer + MESSAGE_LENGTH, msgLength - MESSAGE_LENGTH);
					USHORT udpPort = atoi(udpPortString.c_str());

					// Construct Message
					const int ACK_MSG_LEN = MESSAGE_LENGTH + ID_LENGTH;
					char joinAckMsg[ACK_MSG_LEN];
					memcpy(joinAckMsg, Messages::JOIN_ACC, MESSAGE_LENGTH);
					Messages::dataByte thisClientsID(socketMap[thisSock]->getNodeID());
					joinAckMsg[MESSAGE_LENGTH] = thisClientsID.signedByte;

					CONNECT_LOG_INFO("Sending Ack Message");
					int bytesSent = send(thisSock, joinAckMsg, ACK_MSG_LEN, 0);
					CONNECT_LOG_INFO("Sent " + std::to_string(bytesSent) + " bytes.");
					if (bytesSent != ACK_MSG_LEN) {
						CONNECT_LOG_ERROR("Failed to send the Join Ack Message. The cient has not been added.");
						continue;
					}

					CONNECT_LOG_INFO("Adding Client to list: " + getAddressAsString(socketMap[thisSock]->getClient()));
					socketMap[thisSock]->updatePort(udpPort);
						
					++clientCount;
					

				} else if (Messages::codesMatch(msgBuffer, msgLength, Messages::EXIT)) {
					CONNECT_LOG_INFO("Exit Message Received. Removing socket.");
					
					closesocket(thisSock);
					FD_CLR(thisSock, &master);
					if (socketMap.find(thisSock) != socketMap.end()) {
						socketMap.erase(thisSock);
					}
				}
			}
		}
	}

	return 0;
}

int ConnectionServer::broadcastClientState() {
	const int CLIENT_COUNT = socketMap.size();
	const int DEF_MSG_LEN_BASE = MESSAGE_LENGTH + ID_LENGTH;

	std::string** messages = new std::string*[CLIENT_COUNT];
	int indexStepper = 0;

	if (thisClient != nullptr) {
		std::string* thisMessage = new std::string(Messages::DEFINE, MESSAGE_LENGTH);
		Messages::dataByte idByte(thisClient->getNodeID());
		thisMessage->append(std::to_string(idByte.signedByte));
		thisMessage->append(thisClient->to_string());

		messages[indexStepper++] = thisMessage;
	}

	for (const auto &socket : socketMap) {
		std::string* thisMessage = new std::string(Messages::DEFINE, MESSAGE_LENGTH);
		Messages::dataByte idByte(socket.second->getNodeID());
		thisMessage->append(std::to_string(idByte.signedByte));
		thisMessage->append(socket.second->to_string());
		
		messages[indexStepper++] = thisMessage;
	}

	for (const auto &socket : socketMap) {
		for (int i = 0; i < CLIENT_COUNT; ++i) {
			std::string* message = messages[i];
			send(socket.first, message->c_str(), message->length(), 0);
		}
	}

	for (int i = 0; i < CLIENT_COUNT; ++i) {
		delete messages[i];
	}
	delete messages;

	return 0;
}

std::vector<ClientNode*>* ConnectionServer::getClientList() {
	if (socketMap.size() < TARGET_PLAYER_COUNT) {
		CONNECT_LOG_ERROR("Invalid client list size. Returning NULLPTR: " + std::to_string(socketMap.size()) + "/" + std::to_string(TARGET_PLAYER_COUNT));
		return nullptr;
	}
	std::vector<ClientNode*>* clientIPList = new std::vector<ClientNode*>;
	clientIPList->reserve(TARGET_PLAYER_COUNT);

	for (const auto &socket : socketMap) {
		clientIPList->emplace_back(socket.second);
	}

	return clientIPList;
}