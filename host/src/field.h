#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <cstdint>
#include <string>

uint32_t byteswap32(uint32_t val);

template<class T> T parse_field(std::vector<char>::const_iterator& buf, const std::vector<char>::const_iterator& end);
template<class T> void serialize_field(const T& value_entA, std::vector<char>& buf);

template<>
unsigned char parse_field<unsigned char>(std::vector<char>::const_iterator& buf, const std::vector<char>::const_iterator& end);
template<>
void serialize_field<unsigned char>(const unsigned char& val, std::vector<char>& buf);
template<>
char parse_field<char>(std::vector<char>::const_iterator& buf, const std::vector<char>::const_iterator& end);
template<>
void serialize_field<char>(const char& val, std::vector<char>& buf);
template<>
uint32_t parse_field<uint32_t>(std::vector<char>::const_iterator& buf, const std::vector<char>::const_iterator& end);
template<>
void serialize_field<uint32_t>(const uint32_t& val, std::vector<char>& buf);
template<>
std::string parse_field<std::string>(std::vector<char>::const_iterator& buf, const std::vector<char>::const_iterator& end);
template<>
void serialize_field<std::string>(const std::string& val, std::vector<char>& buf);




#endif