#ifndef PACKETS_H
#define PACKETS_H

#include <cstdint>
#include <vector>
#include <memory>

class SerializedPacket {
public:
	SerializedPacket(uint8_t type, std::vector<uint8_t> data) : type(type), data(std::move(data)) {}
	uint8_t type;
	std::vector<uint8_t> data;
};

class Packet {
public:
	Packet(uint8_t type) : _type(type) {}
	// static void register_type(
	// 	unsigned char type,
	// 	std::function<std::unique_ptr<Packet>(const std::vector<char>&)> parser
	// );
	// template<class T> static void register_type() {
	// 	register_type(T::typenum, [](const std::vector<char>& data) {
	// 		return std::make_unique<T>(data);
	// 	});
	// }
	virtual SerializedPacket serialize() const = 0;
	uint8_t type() const {
		return _type;
	}
	
protected:
	uint8_t _type;

};

#endif