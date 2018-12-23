#pragma once
#include "pch.h"
#include "IPUtils.h"

class ClientNode {
private:
	static unsigned int lastNodeID;

	char updateValue = '0';

	unsigned int nodeID;
	USHORT port;
	std::string address;
	SOCKADDR_IN client;

public:
	ClientNode(const SOCKADDR_IN& clientObj)
		: client(clientObj), address(IP_Utils::IP_STRING_LENGTH, ' '), nodeID(++lastNodeID) {
		IP_Utils::expandAddress(clientObj, &address, &port);
	};

	static const unsigned int getLastNodeID();

	const std::string to_string() const;

	const USHORT getPort() const;
	const SOCKADDR_IN getClient() const;

	const char getUpdateValue() const;
	void setUpdateValue(const char& newValue);
};

