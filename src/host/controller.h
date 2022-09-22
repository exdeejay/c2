#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <memory>
#include <string>
#include "packetconnection.h"

class PacketConnection;

/**
 * Handles a connection to the host server and provides the main API to send
 * information back to the server.
 */
class Controller {
public:
	/**
	 * Initializes the controller and registers all commands.
	 */
	Controller(std::unique_ptr<PacketConnection> conn);
	/**
	 * The main client/server loop. Will not return.
	 */
	void loop();
	/**
	 * Sends the contents of `audio_buf` in controller.cpp out to the server.
	 */
	void flush_audio_buffer();

	/**
	 * Sends `ret` packet with `retcode`
	 */
	void ret(char retcode);
	/**
	 * Sends `out` packet
	 */
	void print(std::string out);
	/**
	 * Sends `out` packet, terminating with a newline
	 */
	void println(std::string out);
	/**
	 * Sends `err` packet, terminating with a newline
	 */
	void err_println(std::string err);
	/**
	 * Sends `buffer` packet
	 */
	void send_buffer(const std::vector<char> buf);
	/**
	 * Sends `audio` packet
	 */
	void buffer_audio(const char* buf, size_t size);

private:
	std::unique_ptr<PacketConnection> conn;
};

#endif