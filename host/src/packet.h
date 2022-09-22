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
	Packet(unsigned char type) : _type(type) {}
	static void register_type(
		unsigned char type,
		std::function<std::unique_ptr<Packet>(const std::vector<char>&)> parser
	);
	template<class T> static void register_type() {
		register_type(T::typenum, [](const std::vector<char>& data) {
			return std::make_unique<T>(data);
		});
	}
	static void register_handler(unsigned char type, std::function<bool(Controller&, Packet&)>);
	static void handle_packet(Controller& ctrl, Packet& pkt);
	static std::unique_ptr<Packet> parse(const SerializedPacket& spkt);
	virtual SerializedPacket serialize() const = 0;
	unsigned char type() const {
		return _type;
	}
	
protected:
	unsigned char _type;

private:
	static std::unordered_map<unsigned char, std::function<std::unique_ptr<Packet>(const std::vector<char>&)>> registry;
	static std::multimap<unsigned char, std::function<bool(Controller&, Packet&)>> handlers;
};

#endif