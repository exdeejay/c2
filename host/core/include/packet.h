#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>
#include <vector>
#include <memory>

typedef uint8_t packettype_t;

struct SerializedPacket {
	SerializedPacket(packettype_t type, std::vector<uint8_t> data) : type(type), data(std::move(data)) {}
	packettype_t type;
	std::vector<uint8_t> data;
};

class Packet {
public:
	Packet(packettype_t type) : _type(type) {}

	virtual SerializedPacket serialize() const = 0;

	packettype_t type() const {
		return _type;
	}
	
protected:
	packettype_t _type;

};

#endif