#include "controller.h"
#include <iostream>
#include <thread>
#include <memory>
#include "packet.h"
#include "commands.h"
#include "util.h"
using namespace std;

void Controller::consume_packet(const unique_ptr<SerializedPacket> spkt) {
	//unique_ptr<Packet> packet = Packet::create_packet(*spkt.get());
}

void Controller::loop() {
	while (true) {
		unique_ptr<SerializedPacket> spkt = conn->read_packet();
		consume_packet(move(spkt));
	}
}
