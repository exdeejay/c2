#include <Windows.h>
#include <iostream>

#include "packetconnection.h"
#include "controller.h"
#include "commands.h"

using namespace std;

class TestPacket : public Packet {
public:
	unique_ptr<Packet> parse(const std::vector<char>& data) { return make_unique<TestPacket>(); }
	unique_ptr<SerializedPacket> serialize() const { return make_unique<SerializedPacket>(0, make_unique<vector<char>>()); }
};

void hi(int, float) {

}

#ifdef WINGUI
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif

	register_command(hi);
	// string host = "128.211.234.53";
	// int port = 6997;

	// try {
	// 	PacketConnection conn(host, port);
	// 	conn.connect();
	// 	cout << "Connected to " << host << ":"
	// 		<< to_string(port).c_str() << endl;
	// 	Controller controller(conn);
	// 	controller.loop();
	// } catch (asio::system_error& ex) {
	// 	cerr << "Something went wrong: " << ex.what() << endl;
	// }

}
