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

std::string* Messages::construct_DEFINE(const ClientNode* node) {
	const int MESSAGE_BASE_LENGTH = MESSAGE_LENGTH + ID_LENGTH;
	char messageBase[MESSAGE_BASE_LENGTH];
	
	memcpy(messageBase, Messages::DEFINE, MESSAGE_LENGTH);

	Messages::dataByte idByte(node->getNodeID());
	messageBase[MESSAGE_LENGTH] = idByte.signedByte;

	std::string* thisMessage = new std::string(messageBase, MESSAGE_BASE_LENGTH);
	thisMessage->append(node->to_string());

	return thisMessage;
}