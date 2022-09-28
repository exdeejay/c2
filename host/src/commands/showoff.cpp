#include "util.h"
#include <iostream>
#include <windows.h>
#include "controller.h"
#include "commands.h"
using namespace std;

int showoff(Controller& ctrl, unsigned char type) {
	int ret = -1;
	switch (type) {
	case ShowoffCommand::gethacked:
		//ret = getHacked();
		break;
	default:
		//TODO error
		break;
	}
	return ret;
}

int getHacked() {
	if (AllocConsole() == 0) {
		//ctrl.err_println(getWin32ErrorString());
		return -1;
	}
	cout << "test" << endl;
	if (FreeConsole() == 0) {
		//ctrl.err_println(getWin32ErrorString());
		return -2;
	}
	return 0;
}
