#include <iostream>
#include "Server.h"
#include "Client.h"
#include "Peer.h"
#include <string>

void host();
void join();
void peer();

int main() {
	std::string cmd;

	while (true) {
		std::cout << "(H)ost, (J)oin or (P)eer Session?";
		std::cin >> cmd;

		if (cmd == "H") {
			host();
		} else if (cmd == "J") {
			join();
		} else if (cmd == "P") {
			peer();
		} else {
			std::cout << "Unrecognised command" << std::endl;
		}
	}

	return 0;
}

void host() {
	GNAT::Server* server = new GNAT::Server();

	std::string cmd;
	while(true) {
		std::cout << "[Serv] ";
		std::cin >> cmd;

		if (cmd == "conn") {
			server->startConnectionServer();
		} else if (cmd == "startgame") {
			server->startGameServer();
		}
	}

	delete server;
}

void join() {
	GNAT::Client* client = new GNAT::Client();

	std::string cmd;
	while (true) {
		std::cout << "[Clie] ";
		std::cin >> cmd;

		if (cmd == "conn") {
			client->connectToServer();
		} else if (cmd == "startgame") {
			client->startGameServer();
		}
	}

	delete client;
}

void peer() {
	GNAT::Peer* peer = new GNAT::Peer();

	std::string cmd;
	while (true) {
		std::cout << "[Peer] ";
		std::cin >> cmd;

		if (cmd == "host") {
			peer->openAsSessionHost();
		} else if (cmd == "join") {
			peer->connectToSessionHost();
		} else if (cmd == "start") {
			peer->startP2PGame();
		}
	}

	delete peer;
}