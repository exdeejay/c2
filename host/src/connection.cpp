#include "connection.h"

#include <iostream>
#include <vector>

#include "commands.h"
#include "util.h"

using asio::ip::tcp;
void Connection::connect() {
	tcp::resolver resolver(ctx);
	tcp::resolver::results_type endpoints = resolver.resolve(host.c_str(), std::to_string(port).c_str(), ec);
	asio::connect(sock, endpoints, ec);
}

using std::vector;
int Connection::read(callback_t callback, void* context) {
	char buf[1024];
	size_t bytes = sock.read_some(asio::buffer(buf), ec);
	if (ec == asio::error::eof) {
		std::cout << "Connection lost. Terminating." << std::endl;
		return -1;
	}
	else if (ec) {
		throw asio::system_error(ec);
	}

	currentBuf.insert(currentBuf.end(), buf, buf + bytes);

	if (currentBuf.size() >= 4 && currentLen == -1) {
		currentLen = byteswap32(get<uint32_t>(&currentBuf[0]));
	}
	if (currentBuf.size() >= currentLen) {
		vector<char> newBuf;
		newBuf.insert(newBuf.begin(), currentBuf.begin(), currentBuf.begin() + currentLen + 4);
		callback(newBuf, context);
		currentBuf.erase(currentBuf.begin(), currentBuf.begin() + currentLen + 4);
		currentLen = -1;
	}

	return bytes;
}

void Connection::write(std::vector<char> &buf) {
	asio::write(sock, asio::buffer(buf), ec);
}
