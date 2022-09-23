#include "util.h"
#include <iostream>
#include <Windows.h>
#include "controller.h"
using namespace std;

int showoff(Controller& ctrl, unsigned char type) {
	int ret;
	switch (type) {
	case ShowoffCommand::gethacked:
		//ret = getHacked();
		break;
	default:
		//TODO error
		ret = -1;
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
