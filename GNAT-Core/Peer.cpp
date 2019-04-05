#include "Peer.h"

namespace GNAT {
	Peer::Peer() {

	}

	Peer::~Peer() {

	}

	bool Peer::connectToSessionHost() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}
		// Start a new instance of ConnectionPeer
	}

	bool Peer::openAsSessionHost() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}
		// Start new instance of Connection Server
	}

	bool Peer::startP2PGame() {
		if (!logInitialised) {
			GNAT_Log::init_peer();
			logInitialised = true;
		}
		// Start an instance of Game Peer
	}
}