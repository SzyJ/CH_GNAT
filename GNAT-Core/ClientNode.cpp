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
	return address + ":" + std::to_string(port);
}

const USHORT ClientNode::getPort() const {
	return port;
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