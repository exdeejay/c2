#include "connection.h"

#include <iostream>
using namespace std;
using asio::ip::tcp;


void Connection::connect() {
	tcp::resolver resolver(ctx);
	tcp::resolver::results_type endpoints =
		resolver.resolve(host.c_str(), to_string(port).c_str(), ec);
	asio::connect(sock, endpoints);
}

int Connection::read(char* buf, int len) {
	size_t bytes = sock.read_some(asio::buffer(buf, len), ec);
	if (ec == asio::error::eof) {
		cout << "Connection lost. Terminating." << endl;
		return -1;
	} else if (ec) {
		throw asio::system_error(ec);
	}
	return bytes;
}

int Connection::write(const char* buf, int len) {
	return asio::write(sock, asio::buffer(buf, len), ec);
}
