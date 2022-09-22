#ifndef PACKETCONNECTION_H
#define PACKETCONNECTION_H

#include <string>
#include <memory>
#include <mutex>
#include "packet.h"
#include "connection.h"

class SerializedPacket;

/**
 * Wraps a Connection object to provide packet sending and receiving,
 * along with compression.
 */
class PacketConnection {
public:
    /**
     * Creates a wrapped Connection object, but does not attempt connection.
     * Call `connect()` to connect to host.
     */
    PacketConnection(const std::string host, int port) {
        conn = std::make_unique<Connection>(host, port);
    }
    /**
     * Calls `connect()` on wrapped connection object.
     */
    void connect();
    /**
     * Read bytes until a valid packet is received.
     * Does not parse the packet beyond its size.
     * 
     * Packet can be parsed with `Packet::parse()`.
     */
    SerializedPacket read_packet_sync();
    /**
     * Write out serialized packet to host.
     * 
     * Must be serialized beforehand with `Packet::serialize()`.
     */
    void write_packet_sync(const SerializedPacket& spkt);

private:
    std::unique_ptr<Connection> conn;
    int compressed_len = -1;
    std::vector<char> buf;
	std::mutex writeMutex;
};

#endif