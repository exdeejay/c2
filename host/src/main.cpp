#include "controller.h"
#include "log.h"
#ifdef _WIN32
	#include <windows.h>
#endif
using namespace std;

/**
 * Main entry function. Define `WINGUI` to compile into Win32 Subsystem .exe.
 */
#if defined(_WIN32) && defined(WINGUI)
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow) {
#else
int main(int argc, char* argv[]) {
#endif
	//TODO: parse arguments?
	//TODO: handle segfaults?
	DEBUGLOG("Starting...\n");
	Controller controller("127.0.0.1", 6997);
	controller.run();
}
