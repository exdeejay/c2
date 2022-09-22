#include "controller.h"
#include <memory>
#include <iostream>
#include <string>
#include "packet.h"
#include "simplepacket.h"
#include "simplecommand.h"
#include "commands.h"
using namespace std;

void register_all_commands() {
    register_command(3, navigation);
}

void Controller::init() {
	SimplePacket<char>::register_type(0);	//ret
	SimplePacket<string>::register_type(1); //out
	SimplePacket<string>::register_type(2); //err
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

void Controller::println(string out) {
 	SimplePacket<string> pkt(1, out);
 	conn->write_packet_sync(pkt.serialize());
}

void Controller::err_println(string err) {
 	SimplePacket<string> pkt(2, err);
 	conn->write_packet_sync(pkt.serialize());
}
