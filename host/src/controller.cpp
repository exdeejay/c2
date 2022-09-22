#include "controller.h"

#include <iostream>
#include <thread>

#include "packets.h"
#include "commands.h"
#include "util.h"


void Controller::handleData(const std::vector<char>& buf) {
	uint32_t size = get<uint32_t>(&buf[0]);
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
		PAK_CASE(Packet::audioControl):
			ret = audioControl(this, (AudioCommand) get<uint32_t>(&buf[5]));
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

void Controller::sendOut(const std::string out) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+out.size());
	buf.push_back(1);
	writeString(buf, out);
	conn->write(buf);
}

void Controller::sendErr(const std::string err) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+err.size());
	buf.push_back(2);
	writeString(buf, err);
	conn->write(buf);
}

void Controller::sendBuffer(const char* data, size_t size) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+size);
	buf.push_back(3);
	writeBuffer(buf, data, size);
	conn->write(buf);
}

void Controller::sendBuffer(const std::vector<char>& data) {
	std::vector<char> buf;
	writeUInt32(buf, 1 + 4+data.size());
	buf.push_back(3);
	writeBuffer(buf, data);
	conn->write(buf);
}

void Controller::bufferAudio(const char* data, size_t size) {
	for (int i = 0; i < size; i++) {
		audioQueue.push(data[i]);
	}
}

void Controller::flushAudioBuffer() {
	while (true) {
		if (audioQueue.size() > 0) {
			int size = audioQueue.size();
			std::vector<char> buf;
			writeUInt32(buf, 1 + 4+size);
			buf.push_back(4);
			writeUInt32(buf, size);
			for (int i = 0; i < size; i++) {
				buf.push_back(audioQueue.front());
				audioQueue.pop();
			}
			conn->write(buf);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void Controller::loop() {
	std::thread audioFlushThread(&Controller::flushAudioBuffer, this);
	while (true) {
		conn->read(Controller::handleDataCallback, this);
	}
}
