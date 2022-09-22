#include "util.h"


uint32_t byteswap32(uint32_t val) {
	return _byteswap_ulong(val);
}

void writeUInt32(std::vector<char>& buf, int val) {
	uint32_t swapped = byteswap32(val);
	buf.push_back(swapped & 0xFF);
	buf.push_back((swapped >> 8) & 0xFF);
	buf.push_back((swapped >> 16) & 0xFF);
	buf.push_back((swapped >> 24) & 0xFF);
}
