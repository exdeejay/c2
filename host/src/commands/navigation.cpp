#include "commands.h"
#include <iostream>
#include <cstdio>
#include <cstring>
#include <Windows.h>
#include "util.h"
#include "field.h"
using namespace std;

int navigation(Controller& ctrl, unsigned char cmd, vector<char> data) {
	switch (cmd) {
		case 0:
			return listFiles(ctrl, parse_field<string>(data.cbegin(), data.cend()));
		case 1:
			return changeDir(ctrl, parse_field<string>(data.cbegin(), data.cend()));
		case 2:
			return pwd(ctrl);
		case 3:
			return removeFile(ctrl, parse_field<string>(data.cbegin(), data.cend()));
		default:
			//TODO error msg
		return -1;
	}
}

int pwd(Controller& ctrl) {
	char path[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, path);
	ctrl.println(path);
	return 0;
}

int changeDir(Controller& ctrl, const string path) {
	BOOL result = SetCurrentDirectoryA(path.c_str());
	if (!result) {
		ctrl.err_println(getWin32ErrorString());
		return -1;
	}
	return 0;
}

int listFiles(Controller& ctrl, const string path) {
	WIN32_FIND_DATA ffd;
	HANDLE hFind;
	if (path.size() == 0) {
		hFind = FindFirstFileA("*", &ffd);
	} else {
		string searchpath = path + "\\*";
		hFind = FindFirstFileA(searchpath.c_str(), &ffd);
	}

	if (hFind == INVALID_HANDLE_VALUE) {
		return 1;
	}
	
	string output;
	do {
		if (ffd.cFileName[0] == '.' &&
				(ffd.cFileName[1] == '\0' ||
				(ffd.cFileName[1] == '.' && ffd.cFileName[2] == '\0'))) {
			continue;
		}
		output += reinterpret_cast<char*>(ffd.cFileName);
		output += "\n";
	} while (FindNextFileA(hFind, &ffd) != 0);
	FindClose(hFind);
	ctrl.print(output);
	return 0;
}

int removeFile(Controller& ctrl, const string path) {
	return remove(path.c_str());
}
