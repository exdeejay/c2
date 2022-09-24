#ifndef PACKET_CONNECTION_IMPL
#define PACKET_CONNECTION_IMPL

#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include <cstdint>
#include "connection.h"

class PacketConnectionImpl {
public:
    PacketConnectionImpl(std::string host, uint16_t port) : conn(host, port) {}

    Connection conn;
    uint32_t compressed_len = -1;
    std::vector<uint8_t> buf;
	std::mutex writeMutex;
};

#endif