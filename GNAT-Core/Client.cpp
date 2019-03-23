#include "pch.h"
#include "Client.h"
#include "IPUtils.h"
#include "Messages.h"
#include "ConnectionClient.h"
#include "GameClient.h"

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
		ConnectionClinet* conn = new ConnectionClinet();
		int success = 0;

		success = conn->initializeWinSock();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to initialise");
			return false;
		}

		success = conn->connectToServer();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to connect");
			return false;
		}

		success = conn->sendJoinRequest();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to receive: " + std::to_string(success));
			return false;
		}

		CLIENT_LOG_INFO("Successfully joined with ID: " + std::to_string(success));

		delete conn;
		return true;
	}

	bool Client::startGameServer() {
		if (!logInitialised) {
			GNAT_Log::init_client();
			logInitialised = true;
		}
		GameClient* client = new GameClient();
		int success = 0;


		success = client->initializeWinSock();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to initialise");
			return false;
		}

		success = client->startClient();
		if (success < 0) {
			CLIENT_LOG_ERROR("Failed to start game client");
			return false;
		}

		delete client;
		return true;
	}

    const int Client::getErrorCode() const {
        return errorFlag;
    }
}