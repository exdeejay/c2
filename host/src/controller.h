#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <string>
#include "packetconnection.h"

class PacketConnection;
class Controller {
public:
	Controller(std::unique_ptr<PacketConnection> conn) : conn(std::move(conn)) {
		init();
	}
	void init();
	void loop();
	void ret(char retcode);
	void println(std::string out);
	void err_println(std::string err);
private:
	std::unique_ptr<PacketConnection> conn;
};

#endif