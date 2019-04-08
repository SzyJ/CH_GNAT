#pragma once
#include "pch.h"

class IP_Utils {
public:
	static const int IP_STRING_LENGTH = 16;
	static const int PORT_STRING_LENGTH = 5; // 0 to 65,535
	static void expandAddress(SOCKADDR_IN addr, std::string* ipAddress, USHORT* port);
	static void expandAddress(SOCKADDR_IN addr, std::string* ipAddress);

	static int sendMessage(SOCKET socket, SOCKADDR_IN receiver, std::string msg);
	static int sendMessage(SOCKET socket, SOCKADDR_IN receiver, const char* msg, int msgLength);
};