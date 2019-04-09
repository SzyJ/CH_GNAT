#include "pch.h"
#include "ClientNode.h"

byte ClientNode::lastNodeID = 0;

const byte ClientNode::getLastNodeID() {
	return lastNodeID;
}

const byte ClientNode::getNextNodeID() {
	return lastNodeID + (byte) 1;
}

const std::string ClientNode::to_string() const {
	std::string address(IP_Utils::IP_STRING_LENGTH, ' ');
	USHORT port;
	IP_Utils::expandAddress(client, &address, &port);
	return address + ":" + std::to_string(port);
}

const USHORT ClientNode::getPort() const {
	return ntohs(client.sin_port);
}

void ClientNode::updatePort(USHORT newPort) {
	client.sin_port = htons(newPort);
}

const SOCKADDR_IN ClientNode::getClient() const {
	return client;  
}

const char ClientNode::getUpdateValue() const {
	return updateValue;
}

void ClientNode::setUpdateValue(const char& newValue) {
	updateValue = newValue;
}

const byte ClientNode::getNodeID() const {
	return nodeID;
}

void ClientNode::setNodeID(const byte& newID) {
	nodeID = newID;
}