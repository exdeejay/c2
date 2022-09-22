#include "packet.h"
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
using namespace std;

unordered_map<unsigned char, unique_ptr<Packet>(*)(const vector<char>&)> Packet::registry;
multimap<unsigned char, bool(*)(Controller&, Packet&)> Packet::handlers;

void Packet::register_type(unsigned char type, unique_ptr<Packet>(*builder)(const vector<char>&)) {
	registry[type] = builder;
}

void Packet::register_handler(unsigned char type, bool(*handler)(Controller&, Packet&)) {
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
	return registry[spkt.type](*spkt.data.get());
}