#include <iostream>
#include "Server.h"

int main() {
	std::cout << "Hello World" << std::endl;
	GNAT::GNAT_Log::init();


	GNAT::Server* s = new GNAT::Server();

	//LOG_ERROR("This is an Error!");
	//LOG_TRACE("This is a Trace!");
	//LOG_INFO("This is an Info!");

	delete s;
}