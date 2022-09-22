#include <iostream>
#include <asio.hpp>

#include "packets.h"


uint32_t byteswap32(uint32_t val) {
	return _byteswap_ulong(val);
}

template<typename T>
T get(char* buf, int offset) {
	return *((T*)(buf + offset));
}

void handle_data(char* buf, int len) {
	uint32_t size = byteswap32(get<uint32_t>(buf, 0));
	std::cout << "Size of packet: " << size << std::endl;
}

using asio::ip::tcp;
int main(int argc, char* argv[]) {
	asio::io_context ctx;
	
	tcp::resolver resolver(ctx);
	tcp::resolver::results_type endpoints = resolver.resolve("localhost", "4444");
	tcp::socket sock(ctx);
	asio::connect(sock, endpoints);

	char buf[1024];
	asio::error_code err;
	while (true) {	
		size_t len = sock.read_some(asio::buffer(buf), err);
		if (err == asio::error::eof) {
			std::cout << "Connection lost. Terminating." << std::endl;
			break;
		} else if (err) {
			throw asio::system_error(err);
		}

		handle_data(buf, len);
	}
}
