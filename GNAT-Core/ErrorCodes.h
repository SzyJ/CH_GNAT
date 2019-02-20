#pragma once

// Startup
#define STARTUP_SUCCESSFUL 0
#define WINSOCK_STARTUP_FAIL -1
#define BINDING_SOCKET_FAIL -2

// Receiving Messages
#define NO_MESSAGE_TO_RECEIVE 0
#define EXEPTION_DURING_MSG_RECEIVE -3

#define UNEXPECTED_PROGRAM_STATE -99

typedef int Error_Code;