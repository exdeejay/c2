#include "packet.h"
#include <vector>
#include <unordered_map>
#include <memory>
using namespace std;

unordered_map<unsigned char, unique_ptr<Packet>(*)(const vector<char>&)> Packet::registry;

void Packet::register_packet_type(unsigned char type, unique_ptr<Packet>(*builder)(const vector<char>&)) {
	registry[type] = builder;
}

unique_ptr<Packet> Packet::create_packet(const SerializedPacket& spkt) {
	return registry[spkt.type](*spkt.data.get());
}