#include <iostream>
#include "Server.h"
#include "Client.h"

int main() {
	std::cout << "Hello World" << std::endl;
	GNAT::GNAT_Log::init();

	GNAT::Server* s = new GNAT::Server();
	GNAT::Client* c = new GNAT::Client();
	
	//LOG_ERROR("This is an Error!");
	//LOG_TRACE("This is a Trace!");
	//LOG_INFO("This is an Info!");

	delete c;
	delete s;
}