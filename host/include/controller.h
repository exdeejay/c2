#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <queue>

#include "connection.h"


class Controller {
public:
	Controller(Connection& conn) : conn(&conn) {}
	
	static void handleDataCallback(const std::vector<char>& buf, void* context) {
		reinterpret_cast<Controller*>(context)->handleData(buf);
	}

	void handleData(const std::vector<char>& buf);
	void ret(int code);
	void sendOut(const std::string out);
	void sendErr(const std::string err);
	void sendBuffer(const char* data, size_t size);
	void sendBuffer(const std::vector<char>& data);

	void bufferAudio(const char* data, size_t amount);
	void flushAudioBuffer();

	void loop();

private:
	Connection* conn;
	std::queue<char> audioQueue;
};

#endif