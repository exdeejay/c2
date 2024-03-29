#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>

#ifdef _WIN32
	#include <windows.h>
#endif

template<class T>
T byteswap(T val) {
	T swapped = 0;
	for (int i = 0; i < sizeof(T); i++) {
		swapped |= (val & 0xff) << ((sizeof(T)-i-1)*8);
		val >>= 8;
	}
	return swapped;
}

template <typename T = uint64_t>
T decodeVarint(char *input, size_t *offset) {
	T ret = 0;
	size_t i;
	for (i = 0; i < sizeof(T); i++) {
		ret |= (input[i] & 0x7f) << (7 * i);
		if ((input[i] & 0x80) == 0) {
			i++;
			break;
		}
	}
	*offset += i;
	return ret;
}

inline uint64_t combineDWORDs(uint32_t high, uint32_t low) {
	return static_cast<uint64_t>(high) << 32 | static_cast<uint64_t>(low);
}

#ifdef _WIN32
inline std::string getWin32ErrorString() {
	DWORD err = GetLastError();
	char *msgBuf;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
				   nullptr, err, 0, (LPSTR)&msgBuf, 0, nullptr);
	std::string errMsg = msgBuf;
	errMsg.erase(errMsg.end() - 1);
	LocalFree(msgBuf);
	return errMsg;
}
#endif

#endif