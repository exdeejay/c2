#ifndef PACKET_CONNECTION_IMPL
#define PACKET_CONNECTION_IMPL

#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include "connection.h"

class PacketConnectionImpl {
public:
    PacketConnectionImpl(std::string host, uint16_t port) : conn(host, port) {}

    Connection conn;
    int compressed_len = -1;
    std::vector<char> buf;
	std::mutex writeMutex;
};

#endif