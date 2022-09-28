#include <windows.h>
#include "util.h"
#include "controller.h"
using namespace std;

int exec(Controller& ctrl, const string cmd, bool wait) {
	HANDLE inPipe[2];
	HANDLE outPipe[2];

	SECURITY_ATTRIBUTES sa = {};
	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.bInheritHandle = TRUE;
	sa.lpSecurityDescriptor = nullptr;
	CreatePipe(&inPipe[0], &inPipe[1], &sa, 0);
	SetHandleInformation(inPipe[1], HANDLE_FLAG_INHERIT, 0);
	CreatePipe(&outPipe[0], &outPipe[1], &sa, 0);
	SetHandleInformation(outPipe[0], HANDLE_FLAG_INHERIT, 0);

	STARTUPINFOA startupInfo = {};
	startupInfo.cb = sizeof(STARTUPINFOA);
	startupInfo.dwFlags = STARTF_USESTDHANDLES;
	startupInfo.hStdInput = inPipe[0];
	startupInfo.hStdOutput = outPipe[1];
	startupInfo.hStdError = outPipe[1];
	PROCESS_INFORMATION pi;
	BOOL status = CreateProcessA(
		nullptr, (char *)cmd.c_str(), nullptr, nullptr, TRUE,
		/*CREATE_NO_WINDOW*/ 0, nullptr, nullptr, &startupInfo, &pi);
	if (!status) {
		ctrl.err_println(getWin32ErrorString());
		return -1;
	}
	CloseHandle(inPipe[0]);
	CloseHandle(outPipe[1]);
	CloseHandle(inPipe[1]);

	DWORD retCode = 0;
	if (wait) {
		string out;
		while (status) {
			char buf[8192];
			DWORD size;
			status = ReadFile(outPipe[0], buf, 1024, &size, nullptr);
			if (!status) {
				break;
			}
			if (size != (DWORD)-1) {
				out.assign(buf, size);
				ctrl.print(out);
			}
		}

		WaitForSingleObject(pi.hProcess, INFINITE);
		GetExitCodeProcess(pi.hProcess, &retCode);
	}

	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return retCode;
}
