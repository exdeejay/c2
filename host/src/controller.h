#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <queue>
#include "packet.h"
#include "packetconnection.h"


class Controller {
public:
	Controller(PacketConnection& conn) : conn(&conn) {}
	void consume_packet(const std::unique_ptr<SerializedPacket> spkt);
	void loop();
private:
	PacketConnection* conn;
};

#endif