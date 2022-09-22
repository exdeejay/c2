#include "controller.h"

#include "packets.h"
#include "commands.h"
#include "util.h"


void Controller::handleData(std::vector<char>& buf) {
	uint32_t size = byteswap32(get<uint32_t>(&buf[0]));
	uint8_t command = get<uint8_t>(&buf[4]);
	
	switch (command) {
		PAK_CASE(Packet::listFiles):
			listFiles(this);
			break;
	}
}

void Controller::sendOut(std::string out) {
	std::vector<char> buf;
	writeUInt32(buf, out.size() + 5);
	buf.push_back(1);
	writeUInt32(buf, out.size());
	for (char c : out) {
		buf.push_back(c);
	}
	conn->write(buf);
}

void Controller::loop() {
	while (true) {
		conn->read(Controller::handleDataCallback, this);
	}
}
