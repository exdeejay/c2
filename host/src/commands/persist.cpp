#include <string>
#include <Windows.h>
#include "util.h"
#include "controller.h"
using namespace std;

int persist(Controller& ctrl, const string regkey) {
	PHKEY curr_user_key;
	LSTATUS result;
	// result = RegOpenCurrentUser(nullptr, &curr_user_key);
	// if (result != ERROR_SUCCESS) {
	// 	ctrl.err_println(getWin32ErrorString());
	// 	return -1;
	// }
	
	HKEY runkey;
	result = RegOpenKeyExA(
		HKEY_CURRENT_USER, //curr_user_key,
		"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run",
		0,
		KEY_WRITE,
		&runkey
	);
	if (result != ERROR_SUCCESS) {
		ctrl.err_println(getWin32ErrorString());
		return -2;
	}
	
	char path[MAX_PATH+2];
	path[0] = '"';
	DWORD len = GetModuleFileNameA(nullptr, path + 1, MAX_PATH);
	path[len+1] = '"';
	path[len+2] = '\0';
	result = RegSetKeyValue(runkey, nullptr, regkey.c_str(), REG_SZ, path, len+3);
	if (result != ERROR_SUCCESS) {
		ctrl.err_println(getWin32ErrorString());
		return -3;
	}
	return 0;
}
