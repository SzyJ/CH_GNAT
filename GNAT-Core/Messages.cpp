#include "pch.h"
#include "Messages.h"

const char* Messages::JOIN_REQ = "JR";
const char* Messages::JOIN_ACC = "JA";
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