#include <memory>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <thread>
#include <cstdint>
#include <stdexcept>
#include <thread>

#include <asio.hpp>
#include "controller.h"
#include "controllerimpl.h"
#include "log.h"
#include "simplepacket.h"
#include "plugin.h"

using namespace std;


/**
 * Lock-free queue to hold audio data from mic.
 * Used to store data without processing it in the high-priority audio thread.
 * Flushed every so often to the server.
 */
// boost::lockfree::spsc_queue<char, boost::lockfree::capacity<8192>> audio_buf;

Controller::Controller(string host, uint16_t port) : impl(make_unique<ControllerImpl>(host, port)) {}

Controller::~Controller() = default;


void Controller::run() {
	while (true) {
		try {
			impl->conn = make_unique<PacketConnection>(impl->host, impl->port);
			impl->conn->connect();
			DEBUGLOG("Starting packet read loop\n");
			while (true) {
				SerializedPacket spkt = impl->conn->read_packet_sync();
				auto packet = parse_packet(spkt);
				handle_packet(*packet);
			}
		} catch (asio::system_error ec) {
			DEBUGLOG("ASIO system error: %s\n", ec.what());
			this_thread::sleep_for(1s);
		}
	}
}

void Controller::send_packet(Packet& packet) {
	impl->conn->write_packet_sync(packet.serialize());
}

void Controller::ret(retcode_t retcode) {
	SimplePacket<retcode_t> pkt(0, retcode);
	impl->conn->write_packet_sync(pkt.serialize());
}

void Controller::print(string out) {
	SimplePacket<string> pkt(1, out);
	send_packet(pkt);
}

void Controller::println(string out) {
	out += "\n";
	print(out);
}

void Controller::err_println(string err) {
	SimplePacket<string> pkt(2, err);
	send_packet(pkt);
}

void Controller::send_buffer(const vector<uint8_t> buf) {
	SimplePacket<vector<uint8_t>> pkt(3, buf);
	send_packet(pkt);
}

void Controller::register_plugin(unique_ptr<Plugin> builder) {
	builder->init(*this);
	impl->plugins.push_back(std::move(builder));
}

void Controller::register_command(unique_ptr<CommandBuilder> builder) {
	auto fn_pair = builder->build();
	register_type(builder->type(), fn_pair.first);
	register_handler(builder->type(), fn_pair.second);
	impl->commands.emplace_back(std::move(builder));
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
