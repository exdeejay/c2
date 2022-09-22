#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <vector>
#include <memory>
#include <string>

class ControllerImpl;

/**
 * Handles a connection to the host server and provides the main API to send
 * information back to the server.
 */
class Controller {
public:
	/**
	 * Initializes the controller and registers all commands.
	 */
	Controller(std::string host, short port);

	/**
	 * The main client/server loop. Will not return.
	 */
	void run();

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

	// /**
	//  * Sends `audio` packet
	//  */
	// void buffer_audio(const char* buf, size_t size);

private:
	std::unique_ptr<ControllerImpl> impl;
};

#endif