#ifndef PACKETCONNECTION_H
#define PACKETCONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include "packet.h"
#include "connection.h"

class PacketConnection {
public:
    PacketConnection(const std::string host, int port) {
        conn = std::make_unique<Connection>(host, port);
    }

    void connect();
    std::unique_ptr<SerializedPacket> read_packet();
    void write_packet_sync(const SerializedPacket& spkt);

private:
    std::unique_ptr<Connection> conn;
    int compressed_len = -1;
    std::vector<char> buf;
	std::mutex writeMutex;
};

#endif