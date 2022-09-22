#include "util.h"

#include <iostream>
#include <Windows.h>

using namespace std;

template <> uint32_t get<uint32_t>(const char *buf, int *offset) {
	if (offset == nullptr) {
		return byteswap32(*((uint32_t *)buf));
	} else {
		uint32_t retVal = byteswap32(*((uint32_t *)(buf + *offset)));
		*offset += sizeof(uint32_t);
		return retVal;
	}
}

template <> string get<string>(const char *buf, int *offset) {
	if (offset == nullptr) {
		uint32_t size = get<uint32_t>(buf);
		string str;
		str.insert(str.begin(), buf, buf + size);
		return str;
	} else {
		uint32_t size = get<uint32_t>(buf, offset);
		string str;
		str.insert(str.begin(), buf + *offset, buf + *offset + size);
		*offset += size;
		return str;
	}
}

uint32_t byteswap32(uint32_t val) { return _byteswap_ulong(val); }

void writeUInt32(vector<char> &buf, uint32_t val) {
	uint32_t swapped = byteswap32(val);
	buf.push_back(swapped & 0xFF);
	buf.push_back((swapped >> 8) & 0xFF);
	buf.push_back((swapped >> 16) & 0xFF);
	buf.push_back((swapped >> 24) & 0xFF);
}

void writeString(vector<char> &buf, const string &str) {
	writeUInt32(buf, str.size());
	buf.insert(buf.end(), str.begin(), str.end());
}

void writeBuffer(vector<char> &buf, const char *data, size_t size) {
	writeUInt32(buf, size);
	buf.insert(buf.end(), data, data + size);
}

void writeBuffer(vector<char> &buf, const vector<char> &data) {
	writeUInt32(buf, data.size());
	buf.insert(buf.end(), data.begin(), data.end());
}

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
