#ifndef PACKETS_H
#define PACKETS_H

#include <string>
#include <vector>
#include <memory>

class Packet;
class SerializedPacket {
public:
	SerializedPacket(unsigned char type, std::unique_ptr<std::vector<char>> data) : type(type), data(std::move(data)) {}
	unsigned char type;
	std::unique_ptr<std::vector<char>> data;
};

class Packet {
public:
	//static std::unique_ptr<Packet> create_packet(const SerializedPacket& spacket);
	virtual std::unique_ptr<Packet> parse(const std::vector<char>& data) = 0;
	virtual std::unique_ptr<SerializedPacket> serialize() const = 0;
};

#endif