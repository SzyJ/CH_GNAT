#pragma once

#define MESSAGE_LENGTH 2
#define ID_LENGTH 1

#define SESSION_HOST_TOKEN "SH"

struct Messages {
	static const char* JOIN_REQ;
	static const char* JOIN_ACC;
	static const char* EXIT;
	static const char* PING_REQ;
	static const char* PING_RESP;
	static const char* UPDATE;
	static const char* DEFINE;
	static const char* CURRENT_STATE;

	union dataByte {
		byte unsignedByte; // Unsigned value 0 - 255
		char signedByte; // Signed value -128 - 127

		dataByte()
			: signedByte(0) {}

		dataByte(byte unsignedByte)
			: unsignedByte(unsignedByte) {}

		dataByte(char signedByte)
			: signedByte(signedByte) {}
	};

	static bool codesMatch(const char* message, const int messageLen, const char* code);
};