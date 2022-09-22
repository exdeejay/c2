#ifndef PACKETS_H
#define PACKETS_H

enum class Packet {
	ping,
	stdIn,
	getShell,
	changeDir,
	listFiles
};

#endif