#pragma once
#include "pch.h"
#include "ErrorCodes.h"
#include "ConnectionClient.h"
#include "GameClient.h"

namespace GNAT {
	class Client {
	private:
		bool connectionCompleted = false;
		bool logInitialised = false;
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		byte thisID;
		char const* thisVal;

		GameClient* gameClient = nullptr;

	public:
		Client();
		~Client();

		bool connectToServer();
		bool startGameServer();

		const int getErrorCode() const;
	};
}