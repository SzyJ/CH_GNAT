#pragma once
#include "pch.h"

namespace GNAT {
	class Peer {
	private:
		bool logInitialised = false;

	public:
		Peer();
		~Peer();

		bool connectToSessionHost();
		bool openAsSessionHost();
		bool startP2PGame();
	};
}