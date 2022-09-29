#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <functional>
#include <utility>
#include "command.h"

class ControllerImpl;
typedef uint8_t retcode_t;

/**
 * Handles a connection to the host server and provides the main API to send
 * information back to the server.
 */
class Controller {
public:
	/**
	 * Initializes the controller and registers all commands.
	 */
	Controller(std::string host, uint16_t port);

	~Controller();

	/**
	 * The main client/server loop. Will not return.
	 */
	void run();

	void send_packet(Packet& packet);

	/**
	 * Sends `ret` packet with `retcode`
	 */
	void ret(retcode_t retcode);

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
	void send_buffer(const std::vector<uint8_t> buf);

	void buffer_audio(const uint8_t* data, size_t len);

	template<class... Args> void register_command(CommandBuilder& builder) {
		auto fn_pair = builder.build();
		register_type(type, fn_pair.first);
		register_handler(type, fn_pair.second);
	}

	void register_type(packettype_t type, std::function<std::unique_ptr<Packet>(const std::vector<uint8_t>&)> builder);

	void register_handler(packettype_t type, std::function<bool(Controller&, Packet&)> handler);

	void handle_packet(Packet& pkt);

	std::unique_ptr<Packet> parse_packet(const SerializedPacket& spkt);

private:
	std::unique_ptr<ControllerImpl> impl;
};

#endif