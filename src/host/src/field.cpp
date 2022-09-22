#include "field.h"
#include <Windows.h>
#include <iostream>
using namespace std;

uint32_t byteswap32(uint32_t val) { return _byteswap_ulong(val); }

unsigned char Field<unsigned char>::parse_field(iter_t& buf, const iter_t& end) {
	return (unsigned char) *buf++;
}
void Field<unsigned char>::serialize_field(const unsigned char& val, vector<char>& buf) {
	buf.push_back(val);
}
char Field<char>::parse_field(iter_t& buf, const iter_t& end) {
	return *buf++;
}
void Field<char>::serialize_field(const char& val, vector<char>& buf) {
	buf.push_back(val);
}

bool Field<bool>::parse_field(iter_t& buf, const iter_t& end) {
	return Field<char>::parse_field(buf, end) != 0;
}
void Field<bool>::serialize_field(const bool& val, std::vector<char>& buf) {
	Field<char>::serialize_field((char) val, buf);
}

uint32_t Field<uint32_t>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t val = byteswap32(*(uint32_t*) (&*buf));
	buf += sizeof(uint32_t);
	return val;
}
void Field<uint32_t>::serialize_field(const uint32_t& val, vector<char>& buf) {
	buf.push_back((val >> 3*8) & 0xFF);
	buf.push_back((val >> 2*8) & 0xFF);
	buf.push_back((val >> 8) & 0xFF);
	buf.push_back(val & 0xFF);
}

string Field<string>::parse_field(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t size = Field<uint32_t>::parse_field(buf, end);
	string str(buf, buf + size);
	buf += size;
	return str;
}
void Field<string>::serialize_field(const string& val, vector<char>& buf) {
	Field<uint32_t>::serialize_field(val.size(), buf);
	for (char c : val) {
		buf.push_back(c);
	}
}
