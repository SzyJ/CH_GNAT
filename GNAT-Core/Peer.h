#pragma once
#include "pch.h"
#include "ClientNode.h"
#include "GamePeer.h"

namespace GNAT {
	class Peer {
	private:
		std::vector<ClientNode*>* peerIPList;
		GamePeer* gamePeer = nullptr;

		byte thisID;

		bool logInitialised = false;
		bool connectionCompleted = false;

		int initialiseGamePeer();

	public:
		Peer();
		~Peer();

		bool connectToSessionHost();
		bool openAsSessionHost();
		bool startP2PGame();
	};
}