#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <string>

std::string getWin32ErrorString();
uint32_t byteswap32(uint32_t val);
uint64_t combineDWORDs(uint32_t high, uint32_t low);

#endif