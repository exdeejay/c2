#include <Windows.h>
#include <iostream>
#include <memory>
#include <exception>
#include "controller.h"
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
	Controller controller("10.0.0.67", 6997);
	controller.run();
}
