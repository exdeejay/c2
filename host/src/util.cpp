#include "util.h"


using namespace std;

template<>
string get<string>(char* buf, int offset) {
	uint32_t size = byteswap32(get<uint32_t>(buf));
	string str;
	for (int i = 0; i < size; i++) {
		str += buf[i + 4];
	}
	return str;
}


uint32_t byteswap32(uint32_t val) {
	return _byteswap_ulong(val);
}

void writeUInt32(vector<char>& buf, int val) {
	uint32_t swapped = byteswap32(val);
	buf.push_back(swapped & 0xFF);
	buf.push_back((swapped >> 8) & 0xFF);
	buf.push_back((swapped >> 16) & 0xFF);
	buf.push_back((swapped >> 24) & 0xFF);
}

void writeString(vector<char>& buf, string& str) {
	writeUInt32(buf, str.size());
	buf.insert(buf.end(), str.begin(), str.end());
}

void writeBuffer(vector<char>& buf, vector<char>& data) {
	writeUInt32(buf, data.size());
	buf.insert(buf.end(), data.begin(), data.end());
}
