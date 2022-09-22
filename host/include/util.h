#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>


template<typename T>
T get(char* buf, int offset = 0) {
	return *((T*)(buf + offset));
}

template<>
std::string get<std::string>(char* buf, int offset);


uint32_t byteswap32(uint32_t val);
void writeUInt32(std::vector<char>& buf, int val);
void writeString(std::vector<char>& buf, std::string& str);
void writeBuffer(std::vector<char>& buf, std::vector<char>& data);



#endif