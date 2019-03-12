#pragma once

// Startup
#define STARTUP_SUCCESSFUL 0
#define WINSOCK_STARTUP_FAIL -1
#define SOCKET_CREATION_FAIL -2
#define BINDING_SOCKET_FAIL -3

// Receiving Messages
#define NO_MESSAGE_TO_RECEIVE 0
#define EXEPTION_DURING_MSG_RECEIVE -4

#define UNEXPECTED_PROGRAM_STATE -99

typedef int Error_Code;