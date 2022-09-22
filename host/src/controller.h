#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include "packetconnection.h"

class PacketConnection;
class Controller {
public:
	Controller(std::unique_ptr<PacketConnection> conn) : conn(std::move(conn)) {
		init();
	}
	void init();
	void loop();
private:
	std::unique_ptr<PacketConnection> conn;
};

#endif