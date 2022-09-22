#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>
#include <cstdint>
#include <string>
#include <vector>


template<typename T>
T get(const char* buf, int offset = 0) {
	return *((T*)(buf + offset));
}

template<>
uint32_t get<uint32_t>(const char* buf, int offset);
template<>
std::string get<std::string>(const char* buf, int offset);


uint32_t byteswap32(uint32_t val);
void writeUInt32(std::vector<char>& buf, uint32_t val);
void writeString(std::vector<char>& buf, const std::string& str);
void writeBuffer(std::vector<char>& buf, const char* data, size_t size);
void writeBuffer(std::vector<char>& buf, const std::vector<char>& data);


#endif