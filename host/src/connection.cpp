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

/*

	char buf[1024];
	vector<char> out = { 0, 0, 0, 0 };
	z_stream stream;
	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	deflateInit(&stream, Z_DEFAULT_COMPRESSION);
	stream.avail_in = data.size();
	stream.next_in = (Bytef*) data.data();
	do {
		stream.avail_out = sizeof(buf);
		stream.next_out = (Bytef*) buf;
		deflate(&stream, Z_FINISH);
		out.insert(out.end(), buf, buf + (sizeof(buf) - stream.avail_out));
	} while (stream.avail_out == 0);
	deflateEnd(&stream);

	unsigned int size = byteswap32(out.size() - 4);
	out[0] = size & 0xFF;
	out[1] = (size >> 8) & 0xFF;
	out[2] = (size >> 16) & 0xFF;
	out[3] = (size >> 24) & 0xFF;
*/