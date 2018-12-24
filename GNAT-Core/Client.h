#pragma once
#include "pch.h"
#include "ErrorCodes.h"
namespace GNAT {
	class Client {
	private:
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		const static std::string SERVER_ADDRESS;
		const static USHORT SERVER_PORT;

		SOCKADDR_IN serverAddress;

		SOCKET clientSocket;


	public:
		Client();
		~Client();

		const int getErrorCode() const;
	};
}