#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <functional>

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
	void ret(uint8_t retcode);

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

	// /**
	//  * Sends `audio` packet
	//  */
	// void buffer_audio(const char* buf, size_t size);

	template<class... Args> void register_command(unsigned char type, int(*command)(Controller&, Args...)) {
		CommandPacket<Args...>::build(type, command);
	}


	void register_type(uint8_t type, std::function<Packet(const std::vector<uint8_t>&)> builder);
	// {
	// 	registry[type] = builder;
	// }

	void register_handler(uint8_t type, std::function<bool(Controller&, Packet&)> handler);
	// {
	// 	handlers.insert(make_pair(type, handler));
	// }

	void handle_packet(Packet& pkt);
	// {
	// 	auto range = handlers.equal_range(pkt.type());
	// 	for (auto i = range.first; i != range.second; i++) {
	// 		if (!i->second(ctrl, pkt)) {
	// 			return;
	// 		}
	// 	}
	// }

private:
	std::unique_ptr<ControllerImpl> impl;
};

#endif