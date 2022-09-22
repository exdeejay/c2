#include "util.h"
#include <Windows.h>
#include <string>
using namespace std;

string getWin32ErrorString() {
	DWORD err = GetLastError();
	char *msgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				   nullptr, err, 0, (LPSTR)&msgBuf, 0, nullptr);
	string errMsg = msgBuf;
	errMsg.erase(errMsg.end() - 1);
	LocalFree(msgBuf);
	return errMsg;
}