#include "connection.h"

#include <iostream>
#include <vector>
#include <zlib.h>

#include "commands.h"
#include "util.h"

using namespace std;
using asio::ip::tcp;


void Connection::connect() {
	tcp::resolver resolver(ctx);
	tcp::resolver::results_type endpoints =
		resolver.resolve(host.c_str(), std::to_string(port).c_str(), ec);
	asio::connect(sock, endpoints);
}

int Connection::read(callback_t callback, void *context) {
	char buf[1024];
	size_t bytes = sock.read_some(asio::buffer(buf), ec);
	if (ec == asio::error::eof) {
		cout << "Connection lost. Terminating." << endl;
		return -1;
	} else if (ec) {
		throw asio::system_error(ec);
	}

	currentBuf.insert(currentBuf.end(), buf, buf + bytes);

	if (currentBuf.size() >= 4 && currentLen == -1) {
		currentLen = get<uint32_t>(&currentBuf[0]);
	}
	if (currentLen != -1 && currentBuf.size() >= currentLen + 4) {
		vector<char> deflated;
		char deflateBuf[1024];

		z_stream stream;
		stream.zalloc = Z_NULL;
		stream.zfree = Z_NULL;
		stream.opaque = Z_NULL;
		stream.avail_in = 0;
		stream.next_in = Z_NULL;
		inflateInit(&stream);
		
		stream.avail_in = currentLen;
		stream.next_in = (Bytef*) currentBuf.data() + 4;
		bool valid = true;
		do {
			stream.avail_out = sizeof(deflateBuf);
			stream.next_out = (Bytef*) deflateBuf;
			int ret = inflate(&stream, Z_FINISH);
			if (ret != Z_OK && ret != Z_STREAM_END) {
				valid = false;
				break;
			}
			deflated.insert(deflated.end(), deflateBuf, deflateBuf + (sizeof(deflateBuf) - stream.avail_out));
		} while (stream.avail_out == 0);
		inflateEnd(&stream);
		if (valid) {
			callback(deflated, context);
		}

		currentBuf.erase(currentBuf.begin(),
						 currentBuf.begin() + currentLen + 4);
		currentLen = -1;
	}

	return bytes;
}

void Connection::write(vector<char> &data) {
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

	lock_guard<mutex> guard(writeMutex);
	asio::write(sock, asio::buffer(out), ec);
}
