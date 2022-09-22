#include "commands.h"
#include <Windows.h>
#include <string>
#include <iostream>
using namespace std;

int dialog(Controller& ctrl, uint32_t type, string msg, string title) {
	return MessageBoxA(nullptr, msg.c_str(), title.c_str(), type);
}