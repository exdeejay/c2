#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <mutex>
#include <asio.hpp>

typedef void (*callback_t)(const std::vector<char>&, void*);

class Connection {
public:
	Connection(const std::string host, const int port) : host(host), port(port), sock(ctx) {}

	void connect();
	int read(callback_t callback, void* context);
	void write(std::vector<char>& buf);

	asio::error_code getLastError() {
		return ec;
	}

	asio::ip::tcp::socket& getSocket() {
		return sock;
	}
private:
	std::string host;
	int port;

	asio::io_context ctx;
	asio::ip::tcp::socket sock;
	asio::error_code ec;

	int currentLen = -1;
	std::vector<char> currentBuf;

	std::mutex writeMutex;
};

#endif
