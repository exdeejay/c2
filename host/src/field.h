#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <cstdint>
#include <string>

uint32_t byteswap32(uint32_t val);

template<class T> T parse_field(std::vector<char>::const_iterator& buf);

template<>
unsigned char parse_field<unsigned char>(std::vector<char>::const_iterator& buf);
template<>
uint32_t parse_field<uint32_t>(std::vector<char>::const_iterator& buf);
template<>
std::string parse_field<std::string>(std::vector<char>::const_iterator& buf);

#endif