#pragma once
#include "ClientNode.h"

#define MESSAGE_LENGTH 2
#define ID_LENGTH 1

#define SESSION_HOST_TOKEN "SH"

struct Messages {
	/*
	 * Message Format:
	 *   JR[Port_For_UDP_Connections]
	 */
	static const char* JOIN_REQ;
	/*
	 * Message Format:
	 *   JA<ID>
	 */
	static const char* JOIN_ACC;
	/*
	 * Message Format:
	 *   EX
	 */
	static const char* EXIT;
	static const char* PING_REQ;
	static const char* PING_RESP;
	/*
	 * Message Format:
	 *   UP[<ID><Current_Value>]...
	 */
	static const char* UPDATE;
	/*
	 * Message Format:
	 *   DF<ID>[Address_String]:[Port]
	 */
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

	static char* construct_DEFINE(const ClientNode* node, int* messageLength);
};