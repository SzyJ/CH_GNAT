#pragma once
#include "pch.h"
#include "ErrorCodes.h"
namespace GNAT {
	class Client {
	private:
		Error_Code errorFlag = STARTUP_SUCCESSFUL;

		const static int PLAYER_COUNT = 4;

		const static std::string SERVER_ADDRESS;
		
		const static USHORT SERVER_PORT;
		const static int MESSAGE_BUFFER_SIZE = 1024;
		char messageBuffer[MESSAGE_BUFFER_SIZE];

		SOCKADDR_IN serverAddress;

		SOCKET clientSocket;

		std::thread sendUpdates;
		std::thread recvUpdates;

		char thisVal;

		bool compareMessageCode(const std::string& message, const std::string& expectedMessage);

		char checkForUserInput() inline;

		bool userInputLoop();
		bool stateUpdateLoop();

	public:
		Client();
		~Client();
		
		bool sendJoinRequest();
		bool startListen();

		const int getErrorCode() const;
	};
}