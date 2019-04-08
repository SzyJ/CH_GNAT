#pragma once
#include "pch.h"
#include "IPUtils.h"

class ClientNode {
private:
	static byte lastNodeID;

	char updateValue = '0';

	byte nodeID;
	SOCKADDR_IN client;

public:
	ClientNode(const SOCKADDR_IN& clientObj)
		: client(clientObj), nodeID(++lastNodeID) {};

	const byte getNodeID() const;
	static const byte getLastNodeID();
	static const byte getNextNodeID();

	const USHORT getPort() const;

	/*
	 * Updates the port that is used for this client
	 * 
	 * Parameters:
	 *             newPort Port to update to in host byte order
	 */
	void updatePort(USHORT newPort);
	const SOCKADDR_IN getClient() const;

	const char getUpdateValue() const;
	void setUpdateValue(const char& newValue);

	const std::string to_string() const;
};

