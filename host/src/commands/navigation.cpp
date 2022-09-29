#include <iostream>
#include <cstdio>
#include <cstring>
#include <vector>
#include <exception>
#include <windows.h>
#include "util.h"
#include "field.h"
#include "controller.h"
#include "datatypes.h"
#include "simplepacket.h"
#include "log.h"
using namespace std;

int pwd(Controller& ctrl) {
	DWORD length = GetCurrentDirectoryA(0, nullptr);
	char* path = new char[length + 1];
	GetCurrentDirectoryA(length+1, path);
	ctrl.println(path);
	delete[] path;
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
	
	vector<FileData> files;
	do {
		if (ffd.cFileName[0] == '.' &&
				(ffd.cFileName[1] == '\0' ||
				(ffd.cFileName[1] == '.' && ffd.cFileName[2] == '\0'))) {
			continue;
		}
		FileData file;
		file.name = ffd.cFileName;
		file.attrs = ffd.dwFileAttributes;
		file.creation_time = combineDWORDs(ffd.ftCreationTime.dwHighDateTime, ffd.ftCreationTime.dwLowDateTime);
		file.last_access_time = combineDWORDs(ffd.ftLastAccessTime.dwHighDateTime, ffd.ftLastAccessTime.dwLowDateTime);
		file.last_write_time = combineDWORDs(ffd.ftLastWriteTime.dwHighDateTime, ffd.ftLastWriteTime.dwLowDateTime);
		file.size = combineDWORDs(ffd.nFileSizeHigh, ffd.nFileSizeLow);
		files.push_back(file);
	} while (FindNextFileA(hFind, &ffd) != 0);
	FindClose(hFind);
	SimplePacket<vector<FileData>> filesPacket(5, files);
	ctrl.send_packet(filesPacket);
	return 0;
}

int removeFile(Controller& ctrl, const string path) {
	return remove(path.c_str());
}

int navigation(Controller& ctrl, uint8_t cmd, string path) {
	switch (cmd) {
		case 0:
			return listFiles(ctrl, path);
		case 1:
			return changeDir(ctrl, path);
		case 2:
			return pwd(ctrl);
		case 3:
			return removeFile(ctrl, path);
		default:
			throw new exception("invalid navigation command");
		return -1;
	}
}
