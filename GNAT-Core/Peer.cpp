#include "pch.h"
#include "Peer.h"
#include "ConnectionServer.h"
#include "ConnectionClient.h"

namespace GNAT {
	Peer::Peer() {

	}

	Peer::~Peer() {

	}

	int Peer::initialiseGamePeer() {
		gamePeer = new GamePeer();
		return gamePeer->initializeWinSock();
	}

	bool Peer::connectToSessionHost() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}

		// Create GamePeer instance to get Port
		int initResult = initialiseGamePeer();
		if (initResult < 0) {
			return false;
		}

		USHORT udpPort = (USHORT)initResult;
		
		// Start a new instance of Connection Client
		ConnectionClient* connectionClient = new ConnectionClient();
		int success = 0;

		success = connectionClient->initializeWinSock();
		if (success < 0) {
			PEER_LOG_ERROR("Failed to initialise TCP");
			return false;
		}

		success = connectionClient->connectToServer();
		if (success < 0) {
			PEER_LOG_ERROR("Failed to connect");
			return false;
		}

		success = connectionClient->sendJoinRequest(udpPort);
		if (success < 0) {
			PEER_LOG_ERROR("Failed to receive: " + std::to_string(success));
			return false;
		}

		thisID = (byte)success;
		PEER_LOG_INFO("Successfully joined with ID: " + std::to_string(success));

		success = connectionClient->listenForPeerInfo();
		if (success < 0) {
			PEER_LOG_ERROR("Failed to get client list from server");
			return false;
		}

		peerIPList = connectionClient->getClientList();
		if (peerIPList == nullptr) {
			PEER_LOG_ERROR("Failed to get client list from server");
			return false;
		}

		connectionCompleted = true;

		delete connectionClient;
		return true;
	}

	bool Peer::openAsSessionHost() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}

		// Create GamePeer instance to get Port
		int initResult = initialiseGamePeer();
		if (initResult < 0) {
			return false;
		}

		USHORT udpPort = (USHORT)initResult; // !!!TODO: Add this client to connection server!!!!

		// Start new instance of Connection Server
		ConnectionServer* connectionServer = new ConnectionServer();
		PEER_LOG_INFO("Starting Winsock...");
		connectionServer->initializeWinSock();

		PEER_LOG_INFO("Establishing TCP Connection..");
		connectionServer->establishTCPConnection();

		peerIPList = connectionServer->getClientList();

		PEER_LOG_INFO("Killing Connection Server..");
		delete connectionServer;
		return false;
	}

	bool Peer::startP2PGame() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}
		

		return false;
	}
}