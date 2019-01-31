#pragma once
#include "pch.h"

class IP_Utils {
public:
	static const int IP_STRING_LENGTH = 16;
	static void expandAddress(SOCKADDR_IN addr, std::string* ipAddress, USHORT* port);

	static int sendMessage(SOCKET socket, SOCKADDR_IN receiver, std::string msg);
	static int sendMessage(SOCKET socket, SOCKADDR_IN receiver, const char* msg, int msgLength);
};