#include "controller.h"

#include <iostream>
#include "packets.h"
#include "commands.h"
#include "util.h"


void Controller::handleData(std::vector<char>& buf) {
	uint32_t size = byteswap32(get<uint32_t>(&buf[0]));
	uint8_t command = get<uint8_t>(&buf[4]);
	
	int ret = -1;
	switch (command) {
		PAK_CASE(Packet::changeDir):
			ret = changeDir(this, get<std::string>(&buf[5]));
			break;
		PAK_CASE(Packet::listFiles):
			ret = listFiles(this, get<std::string>(&buf[5]));
			break;
		PAK_CASE(Packet::pwd):
			ret = pwd(this);
			break;
		PAK_CASE(Packet::screenshot):
			ret = screenshot(this);
			break;
	}

	this->ret(ret);
}

void Controller::ret(int code) {
	std::vector<char> buf;
	writeUInt32(buf, 2);
	buf.push_back(0);
	buf.push_back(code);
	conn->write(buf);
}

void Controller::sendOut(std::string out) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+out.size());
	buf.push_back(1);
	writeString(buf, out);
	conn->write(buf);
}

void Controller::sendErr(std::string err) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+err.size());
	buf.push_back(2);
	writeString(buf, err);
	conn->write(buf);
}

void Controller::sendBuffer(std::vector<char>& data) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+data.size());
	buf.push_back(3);
	writeBuffer(buf, data);
	conn->write(buf);
}

void Controller::loop() {
	while (true) {
		conn->read(Controller::handleDataCallback, this);
	}
}
