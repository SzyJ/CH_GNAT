#pragma once
#include "pch.h"
#include "ClientNode.h"
#include "ErrorCodes.h"

namespace GNAT {
	class Server {
	private:
		bool logInitialised = false;
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		std::vector<ClientNode*>* clientIPList;

	public:
		Server();
		~Server();

		void startConnectionServer();
		void startGameServer();

		const int getErrorCode() const;
	};
}