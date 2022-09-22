#ifndef UTIL_H
#define UTIL_H

#include <vector>
#include <cstdlib>
#include <cstdint>

uint32_t byteswap32(uint32_t val);
void writeUInt32(std::vector<char>& buf, int val);

template<typename T>
T get(char* buf, int offset = 0) {
	return *((T*)(buf + offset));
}


#endif