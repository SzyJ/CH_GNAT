#include "pch.h"
#include "IPUtils.h"

void IP_Utils::expandAddress(SOCKADDR_IN addr, std::string* ipAddress, USHORT* port) {
	char addrBuffer[IP_STRING_LENGTH];
	ZeroMemory(addrBuffer, IP_STRING_LENGTH);
	inet_ntop(AF_INET, &addr.sin_addr, addrBuffer, IP_STRING_LENGTH);

	int realLen;
	for (realLen = 0; realLen < IP_STRING_LENGTH; realLen++) {
		if (addrBuffer[realLen] == '\00') {
			break;
		}
	}

	*ipAddress = std::string(addrBuffer, realLen);
	*port = ntohs(addr.sin_port);
}

int IP_Utils::sendMessage(SOCKET socket, SOCKADDR_IN receiver, std::string msg) {
	return sendMessage(socket, receiver, msg.c_str(), msg.length());
}

int IP_Utils::sendMessage(SOCKET socket, SOCKADDR_IN receiver, const char* msg, int msgLength) {
	return sendto(socket, msg, msgLength, 0, (sockaddr*)&receiver, sizeof(receiver));
}