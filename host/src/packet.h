#ifndef PACKETS_H
#define PACKETS_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <map>
#include "controller.h"

class SerializedPacket {
public:
	SerializedPacket(unsigned char type, std::unique_ptr<std::vector<char>> data) : type(type), data(std::move(data)) {}
	unsigned char type;
	std::unique_ptr<std::vector<char>> data;
};

class Controller;
class Packet {
public:
	static void register_type(unsigned char type, std::unique_ptr<Packet>(*builder)(const std::vector<char>&));
	static void register_handler(unsigned char type, bool(*handler)(Controller&, Packet&));
	static void handle_packet(Controller& ctrl, Packet& pkt);
	static std::unique_ptr<Packet> parse(const SerializedPacket& spkt);
	virtual std::unique_ptr<SerializedPacket> serialize() const = 0;
	virtual unsigned char type() const = 0;
private:
	static std::unordered_map<unsigned char, std::unique_ptr<Packet>(*)(const std::vector<char>&)> registry;
	static std::multimap<unsigned char, bool(*)(Controller&, Packet&)> handlers;
};

#endif