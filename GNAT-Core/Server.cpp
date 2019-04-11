#include "pch.h"
#include "Server.h"
#include "ConnectionServer.h"
#include "GameServer.h"
#include "ServerConfigs.h"

namespace GNAT {
	Server::Server() {
		SetConsoleTitleA(("Server [" + std::to_string(SERVER_PORT) + "]").c_str());
	}

	Server::~Server() {

	}

	void Server::startConnectionServer() {
		if (!logInitialised) {
			GNAT_Log::init_server();
			logInitialised = true;
		}
		ConnectionServer* connServer = new ConnectionServer();

		SERVER_LOG_INFO("Starting Winsock...");
		connServer->initializeWinSock();

		SERVER_LOG_INFO("Establishing TCP Connection..");
		connServer->establishTCPConnection();

		clientIPList = connServer->getClientList();

		SERVER_LOG_INFO("Killing Connection Server..");
		delete connServer;
	}

	void Server::startGameServer() {
		if (!logInitialised) {
			GNAT_Log::init_server();
			logInitialised = true;
		}
		if (clientIPList == NULL) {
			SERVER_LOG_ERROR("Null Client list. Run connection server first.");
			return;
		}

		GameServer* gameServer = new GameServer();

		SERVER_LOG_INFO("Starting Winsock...");
		gameServer->initializeWinSock();

		SERVER_LOG_INFO("Passing in the client list");
		gameServer->setClientList(clientIPList); // TODO!

		SERVER_LOG_INFO("Starting Servever...");
		gameServer->startServer();
	}

	const int Server::getErrorCode() const {
		return errorFlag;
	}
}