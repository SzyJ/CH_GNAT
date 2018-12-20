#include <iostream>
#include "Server.h"

int main() {
	std::cout << "Hello World" << std::endl;
	GNAT::GNAT_Log::init();


	GNAT::Server* s = new GNAT::Server();

	GNAT_ERROR("This is an Error!");
	GNAT_TRACE("This is a Trace!");
	GNAT_INFO("This is an Info!");

	delete s;
}