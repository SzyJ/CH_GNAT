#include "pch.h"
#include "ClientNode.h"

unsigned int ClientNode::lastNodeID = 0;

const unsigned int ClientNode::getLastNodeID() {
	return lastNodeID;
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