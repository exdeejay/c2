#include <string>
#include <iostream>
#include "commands.h"
#include "controller.h"
#ifdef _WIN32
	#include <windows.h>
#endif
using namespace std;

int DialogCommand::execute(Controller& ctrl, uint32_t type, string msg, string title) {
#ifdef _WIN32
	return MessageBoxA(nullptr, msg.c_str(), title.c_str(), type);
#else
	return -1;
#endif
}