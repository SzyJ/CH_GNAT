#pragma once
#include "pch.h"
#include "IPUtils.h"

class ClientNode {
private:
	static byte lastNodeID;

	char updateValue = '0';

	byte nodeID;
	USHORT port;
	std::string address;
	SOCKADDR_IN client;

public:
	ClientNode(const SOCKADDR_IN& clientObj)
		: client(clientObj), address(IP_Utils::IP_STRING_LENGTH, ' '), nodeID(++lastNodeID) {
		IP_Utils::expandAddress(clientObj, &address, &port);
	};

	static const byte getLastNodeID();
	static const byte getNextNodeID();

	const std::string to_string() const;

	const USHORT getPort() const;
	const SOCKADDR_IN getClient() const;

	const char getUpdateValue() const;
	void setUpdateValue(const char& newValue);

	const byte getNodeID() const;
};

