#include <Windows.h>
#include "controller.h"
#include "log.h"
#include <asio.hpp>
using namespace std;

/**
 * Main entry function. Define `WINGUI` to compile into Win32 Subsystem .exe.
 */
#ifdef WINGUI
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
	//TODO: parse arguments?
	//TODO: handle segfaults?
	DEBUGLOG("Starting...\n");
	Controller controller("127.0.0.1", 6997);
	try {
		controller.run();
	} catch (asio::system_error ec) {
		DEBUGLOG("ASIO system error: %s\n", ec.what());
	}
}
