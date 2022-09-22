#include "packet.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <functional>
#include <iostream>
using namespace std;

unordered_map<unsigned char, function<unique_ptr<Packet>(const vector<char>&)>> Packet::registry;
multimap<unsigned char, function<bool(Controller&, Packet&)>> Packet::handlers;

void Packet::register_type(unsigned char type, function<unique_ptr<Packet>(const vector<char>&)> builder) {
	registry[type] = builder;
}

void Packet::register_handler(unsigned char type, function<bool(Controller&, Packet&)> handler) {
	handlers.insert(make_pair(type, handler));
}

void Packet::handle_packet(Controller& ctrl, Packet& pkt) {
	auto range = handlers.equal_range(pkt.type());
	for (auto i = range.first; i != range.second; i++) {
		if (!i->second(ctrl, pkt)) {
			return;
		}
	}
}

unique_ptr<Packet> Packet::parse(const SerializedPacket& spkt) {
	auto pktctr = registry.find(spkt.type);
	if (pktctr == registry.end()) {
		throw new exception("bad packet");
	}
	return pktctr->second(*spkt.data.get());
}