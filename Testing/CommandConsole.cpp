#include <iostream>
#include "Server.h"
#include "Client.h"
#include <string>

void host();
void join();

int main() {
	std::string cmd;

	std::cout << "(H)ost or (J)oin Session? ";
	std::cin >> cmd;

	if (cmd == "H") {
		host();
	} else if (cmd == "J") {
		join();
	} else {
		std::cout << "Unknown command, Exiting...";
		return -1;
	}

	return 0;
}

void host() {
	//GNAT::GNAT_Log::init();
	GNAT::Server* server = new GNAT::Server();

	std::string cmd;
	while(true) {
		std::cout << "[client : host] ";
		std::cin >> cmd;

		/*
		if (cmd == "host") {
			std::cout << "  Listening for Join Requests..." << std::endl;
			server->openToClientConnection();
			std::cout << "  Done finding clinets!" << std::endl;
		} else if (cmd == "start") {
			std::cout << "  Starting listen threads..." << std::endl;
			bool success = server->startServer();
			if (success) {
				std::cout << "  Threads successfully started!" << std::endl;
			} else {
				std::cout << "  Could not start threads. Threads already runnng!" << std::endl;
			}

		} else if (cmd == "stop") {
			std::cout << "  Stopping listen threads..." << std::endl;
			bool success = server->stopServer();
			if (success) {
				std::cout << "  Threads successfully closed!" << std::endl;
			} else {
				std::cout << "  Could not stop threads. Threads are not runnng!" << std::endl;
			}

		}
		else*/
		if (cmd == "conn") {
			server->startConnectionServer();
		} else if (cmd == "startgame") {
			server->startGameServer();
		}
	}

	delete server;
}

void join() {
	//GNAT::GNAT_Log::init();
	GNAT::Client* client = new GNAT::Client();

	std::string cmd;
	while (true) {
		std::cout << "[client : join] ";
		std::cin >> cmd;

		/*
		if (cmd == "join") {
			std::cout << "  Sending join request..." << std::endl;
			bool successful = client->sendJoinRequest();
			
			if (successful) {
				std::cout << "  Message successfuly sent" << std::endl;
				std::cout << "  Join response received" << std::endl;
			} else {
				std::cout << "  Failed to send message or receive response. Exiting..." << std::endl;
				delete client;
				return;
			}
			
			std::cout << "  Listening to the server..." << std::endl;
			client->startListen();

		} else if (cmd == "start") {
			std::cout << "  Starting listen threads..." << std::endl;
			bool success = client->startListen();
			if (success) {
				std::cout << "  Threads successfully started!" << std::endl;
			} else {
				std::cout << "  Could not start threads. Threads already runnng!" << std::endl;
			}

		} else if (cmd == "stop") {
			std::cout << "  Stopping listen threads..." << std::endl;
			bool success = client->stopListen();
			if (success) {
				std::cout << "  Threads successfully closed!" << std::endl;
			} else {
				std::cout << "  Could not stop threads. Threads are not runnng!" << std::endl;
			}

		} else */
		if (cmd == "conn") {
			client->connectToServer();
		} else if (cmd == "startgame") {
			client->startGameServer();
		}
	}

	delete client;
}