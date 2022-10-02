#include <stdio.h>
#include <windows.h>
#include <MemoryModule.h>


#if defined(_WIN32) && defined(WINGUI)
int WinMain(HINSTANCE hInst, HINSTANCE prev, PSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char* argv[])
#endif
{
    HMEMORYMODULE exe = MemoryLoadLibrary(c2_exe, c2_exe_len);
    MemoryCallEntryPoint(exe);
}
