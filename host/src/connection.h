#ifndef CONNECTION_H
#define CONNECTION_H

#include <asio.hpp>
#include <mutex>
#include <string>

/**
 * Wraps an ASIO TCP connection.
 */
class Connection {
public:
	Connection(const std::string host, int port)
		: host(host), port(port), sock(ctx) {}

	void connect();
	int read(char* buf, int len);
	int write(const char* buf, int len);

	asio::error_code getLastError() { return ec; }
	asio::ip::tcp::socket& getSocket() { return sock; }

private:
	std::string host;
	int port;

	asio::io_context ctx;
	asio::ip::tcp::socket sock;
	asio::error_code ec;
};

#endif
