#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include <boost/circular_buffer.hpp>
#include "packetconnection.h"

class PacketConnection;
class Controller {
public:
	Controller(std::unique_ptr<PacketConnection> conn) : conn(std::move(conn)) {
		init();
	}
	void init();
	void loop();
	void flush_audio_buffer();

	void ret(char retcode);
	void print(std::string out);
	void println(std::string out);
	void err_println(std::string err);
	void send_buffer(const std::vector<char> buf);
	void buffer_audio(const char* buf, size_t size);

private:
	std::unique_ptr<PacketConnection> conn;
	std::vector<char> audio_buf;
};

#endif