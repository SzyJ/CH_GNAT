#pragma once
#include "pch.h"
#include "ErrorCodes.h"

namespace GNAT {
	class Client {
	private:
		bool logInitialised = false;
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		byte thisID;
		char const* thisVal;

	public:
		Client();
		~Client();

		bool connectToServer();
		bool startGameServer();

		const int getErrorCode() const;
	};
}