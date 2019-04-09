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

	ClientNode(const byte& id, const char* addr, const USHORT& port)
		: nodeID(id)
	{
		if (id <= 0) {
			nodeID = ++lastNodeID;
		} else {
			lastNodeID = id;
		}

		int clientSize = sizeof(client);
		ZeroMemory(&client, clientSize);

		client.sin_port = htons(port);
		client.sin_family = AF_INET;
		inet_pton(AF_INET, addr, &client.sin_addr);
	}

	void setNodeID(const byte& newID);
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

