#include "commands.h"

#include "Windows.h"


void listFiles(Controller* controller) {
	std::string output;
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile("*", &ffd);
	if (hFind == INVALID_HANDLE_VALUE) {
		//TODO
		return;
	}
	do {
		output += ffd.cFileName;
		output += '\n';
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
	output.erase(output.end() - 1);
	controller->sendOut(output.c_str());
}
