#include "controller.h"
#include <memory>
#include <iostream>
#include "packet.h"
#include "simplecommand.h"
#include "commands.h"
using namespace std;

void register_all_commands() {
    register_command(3, listFiles);
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
