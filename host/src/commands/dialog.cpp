#include <string>
#include <iostream>
#include "controller.h"
#ifdef _WIN32
	#include <Windows.h>
#endif
using namespace std;

int dialog(Controller& ctrl, uint32_t type, string msg, string title) {
#ifdef _WIN32
	return MessageBoxA(nullptr, msg.c_str(), title.c_str(), type);
#else
	return -1;
#endif
}