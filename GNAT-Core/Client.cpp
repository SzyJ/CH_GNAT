#include "pch.h"
#include "Client.h"
#include "IPUtils.h"
#include "Messages.h"

namespace GNAT {
    Client::Client() {
		GNAT_Log::init_client();
    }

    Client::~Client() {

    }

	bool Client::connectToServer() {
		if (!logInitialised) {
			GNAT_Log::init_client();
			logInitialised = true;
		}
		int udpPort = 0;

		gameClient = new GameClient();
		udpPort = gameClient->initializeWinSock();

		if (udpPort < 0) {
			CLIENT_LOG_ERROR("Failed to initialise UDP");
			return false;
		}

		ConnectionClinet* conn = new ConnectionClinet();
		int success = 0;

		success = conn->initializeWinSock();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to initialise TCP");
			return false;
		}

		success = conn->connectToServer();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to connect");
			return false;
		}

		success = conn->sendJoinRequest((u_short) udpPort);
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to receive: " + std::to_string(success));
			return false;
		}

		CLIENT_LOG_INFO("Successfully joined with ID: " + std::to_string(success));

		connectionCompleted = true;
		delete conn;
		return true;
	}

	bool Client::startGameServer() {
		if (!logInitialised) {
			GNAT_Log::init_client();
			logInitialised = true;
		}
		if (!connectionCompleted || gameClient == NULL) {
			CLIENT_LOG_INFO("Can not start game without connection first.");
		}

		
		int success = 0;

		success = gameClient->startClient();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to start game client");
			return false;
		}

		connectionCompleted = false;
		delete gameClient;
		return true;
	}

    const int Client::getErrorCode() const {
        return errorFlag;
    }
}