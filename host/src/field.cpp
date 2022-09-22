#include "field.h"
#include <Windows.h>
#include <iostream>
using namespace std;

using iter_t = vector<char>::const_iterator;
uint32_t byteswap32(uint32_t val) { return _byteswap_ulong(val); }

template <> unsigned char parse_field<unsigned char>(iter_t& buf, const iter_t& end) {
	return (unsigned char) *buf++;
}
template<> void serialize_field<unsigned char>(const unsigned char& val, vector<char>& buf) {
	buf.push_back(val);
}
template <> char parse_field<char>(iter_t& buf, const iter_t& end) {
	return *buf++;
}
template<> void serialize_field<char>(const char& val, vector<char>& buf) {
	buf.push_back(val);
}

template <> uint32_t parse_field<uint32_t>(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t val = byteswap32(*(uint32_t*) (&*buf));
	buf += sizeof(uint32_t);
	return val;
}
template<> void serialize_field<uint32_t>(const uint32_t& val, vector<char>& buf) {
	buf.push_back((val >> 3*8) & 0xFF);
	buf.push_back((val >> 2*8) & 0xFF);
	buf.push_back((val >> 8) & 0xFF);
	buf.push_back(val & 0xFF);
}

template <> string parse_field<string>(iter_t& buf, const iter_t& end) {
	//TODO bounds checking
	uint32_t size = parse_field<uint32_t>(buf, end);
	string str;
	str.insert(str.begin(), buf, buf + size);
	buf += size;
	return str;
}
template<> void serialize_field<string>(const string& val, vector<char>& buf) {
	serialize_field<uint32_t>(val.size(), buf);
	for (char c : val) {
		buf.push_back(c);
	}
}
