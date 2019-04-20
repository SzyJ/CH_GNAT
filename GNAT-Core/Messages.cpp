#include "pch.h"
#include "Messages.h"

const char* Messages::JOIN_REQ = "JR";
const char* Messages::JOIN_ACC = "JA";
const char* Messages::EXIT = "EX";
const char* Messages::PING_REQ = "PQ";
const char* Messages::PING_RESP = "PS";
const char* Messages::UPDATE = "UP";
const char* Messages::DEFINE = "DF";
const char* Messages::CURRENT_STATE = "CS";

bool Messages::codesMatch(const char* message, const int messageLen, const char* code) {
	if (messageLen < MESSAGE_LENGTH) {
		return false;
	}

	for (int charIndex = 0; charIndex < MESSAGE_LENGTH; ++charIndex) {
		if (message[charIndex] != code[charIndex]) {
			return false;
		}
	}

	return true;
}

char* Messages::construct_DEFINE(const ClientNode* node, int* messageLength) {
	Messages::dataByte nodeID(node->getNodeID());
	const std::string addressString = node->to_string();
	const int ADDRESS_LENGTH = addressString.length();
	const int THIS_MESSAGE_LENGTH = MESSAGE_LENGTH + ID_LENGTH + ADDRESS_LENGTH;

	char* message = new char[THIS_MESSAGE_LENGTH];
	//memcpy(message, Messages::DEFINE, MESSAGE_LENGTH);
	message[MESSAGE_LENGTH] = nodeID.signedByte;
	//memcpy(message + MESSAGE_LENGTH + ID_LENGTH, addressString.c_str(), ADDRESS_LENGTH);

	if (messageLength != nullptr) {
		*messageLength = THIS_MESSAGE_LENGTH;
	}

	return message;
}