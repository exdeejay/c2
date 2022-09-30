#include <string>
#include <iostream>

#include "field.h"
#include "util.h"

using namespace std;


template<>
uint8_t Field<uint8_t>::parse_field(iter_t& buf, const iter_t& end) {
	return *buf++;
}
template<>
void Field<uint8_t>::serialize_field(const uint8_t& val, vector<uint8_t>& buf) {
	buf.push_back(val);
}

template<>
bool Field<bool>::parse_field(iter_t& buf, const iter_t& end) {
	return Field<uint8_t>::parse_field(buf, end) != 0;
}
template<>
void Field<bool>::serialize_field(const bool& val, vector<uint8_t>& buf) {
	Field<uint8_t>::serialize_field((uint8_t) val, buf);
}

template<>
int32_t Field<int32_t>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	int32_t val = byteswap(*(int32_t*) (&*buf));
	buf += sizeof(int32_t);
	return val;
}
template<>
void Field<int32_t>::serialize_field(const int32_t& val, vector<uint8_t>& buf) {
	buf.push_back((val >> 3*8) & 0xFF);
	buf.push_back((val >> 2*8) & 0xFF);
	buf.push_back((val >> 8) & 0xFF);
	buf.push_back(val & 0xFF);
}

template<>
uint32_t Field<uint32_t>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t val = byteswap(*(uint32_t*) (&*buf));
	buf += sizeof(uint32_t);
	return val;
}
template<>
void Field<uint32_t>::serialize_field(const uint32_t& val, vector<uint8_t>& buf) {
	buf.push_back((val >> 3*8) & 0xFF);
	buf.push_back((val >> 2*8) & 0xFF);
	buf.push_back((val >> 8) & 0xFF);
	buf.push_back(val & 0xFF);
}

template<>
uint64_t Field<uint64_t>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint64_t val = 0;
	for (int i = 0; i < 8; i++) {
		val <<= 8;
		val |= *buf++;
	}
	return val;
}
template<>
void Field<uint64_t>::serialize_field(const uint64_t& val, vector<uint8_t>& buf) {
	buf.push_back((val >> 7*8) & 0xFF);
	buf.push_back((val >> 6*8) & 0xFF);
	buf.push_back((val >> 5*8) & 0xFF);
	buf.push_back((val >> 4*8) & 0xFF);
	buf.push_back((val >> 3*8) & 0xFF);
	buf.push_back((val >> 2*8) & 0xFF);
	buf.push_back((val >> 8) & 0xFF);
	buf.push_back(val & 0xFF);
}

template<>
string Field<string>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t size = Field<uint32_t>::parse_field(buf, end);
	string str(buf, buf + size);
	buf += size;
	return str;
}
template<>
void Field<string>::serialize_field(const string& val, vector<uint8_t>& buf) {
	Field<uint32_t>::serialize_field(val.size(), buf);
	for (uint8_t c : val) {
		buf.push_back(c);
	}
}
