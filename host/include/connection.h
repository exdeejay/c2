#ifndef CONNECTION_H
#define CONNECTION_H

#include <asio.hpp>
#include <string>
#include <cstdint>

/**
 * Wraps an ASIO TCP connection.
 */
class Connection {
public:
	Connection(const std::string host, uint16_t port)
		: host(host), port(port), sock(ctx) {}

	void connect() {
		asio::ip::tcp::resolver resolver(ctx);
		asio::ip::tcp::resolver::results_type endpoints = resolver.resolve(host.c_str(), std::to_string(port).c_str(), ec);
		asio::connect(sock, endpoints);
	}

	int read(uint8_t* buf, int len) {
		size_t bytes = sock.read_some(asio::buffer(buf, len), ec);
		if (ec == asio::error::eof) {
			return -1;
		} else if (ec) {
			throw asio::system_error(ec);
		}
		return bytes;
	}

	int write(const uint8_t* buf, int len) {
		return asio::write(sock, asio::buffer(buf, len), ec);
	}

	asio::error_code getLastError() { return ec; }
	asio::ip::tcp::socket& getSocket() { return sock; }

private:
	std::string host;
	uint16_t port;

	asio::io_context ctx;
	asio::ip::tcp::socket sock;
	asio::error_code ec;
};

#endif
