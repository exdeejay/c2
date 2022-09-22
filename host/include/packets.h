#ifndef PACKETS_H
#define PACKETS_H

#include <string>
#include <vector>

#define ECASE(type) case (int) type

enum class Packet {
	ping,
	stdIn,
	getShell,
	navigate,
	UNUSED0,
	UNUSED1,
	screenshot,
	audioControl,
	downloadFile,
	uploadFile
};

void handleCommand(std::vector<char>& buf);
void sendOut(std::string out);

#endif