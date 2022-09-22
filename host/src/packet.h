#ifndef PACKETS_H
#define PACKETS_H

#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

class Packet;
class SerializedPacket {
public:
	SerializedPacket(unsigned char type, std::unique_ptr<std::vector<char>> data) : type(type), data(std::move(data)) {}
	unsigned char type;
	std::unique_ptr<std::vector<char>> data;
};

class Packet {
public:
	static void register_packet_type(unsigned char type, std::unique_ptr<Packet>(*builder)(const std::vector<char>&));
	static std::unique_ptr<Packet> create_packet(const SerializedPacket& spkt);
	virtual std::unique_ptr<Packet> parse(const std::vector<char>& data) = 0;
	virtual std::unique_ptr<SerializedPacket> serialize() const = 0;
private:
	static std::unordered_map<unsigned char, std::unique_ptr<Packet>(*)(const std::vector<char>&)> registry;
};

#endif