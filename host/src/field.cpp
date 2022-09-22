#include "field.h"
#include <Windows.h>
#include <iostream>
using namespace std;

uint32_t byteswap32(uint32_t val) { return _byteswap_ulong(val); }

template <> unsigned char parse_field<unsigned char>(vector<char>::const_iterator& buf) {
    return (unsigned char) *buf++;
}

template <> uint32_t parse_field<uint32_t>(vector<char>::const_iterator& buf) {
	uint32_t val = byteswap32(*(uint32_t*)(&*buf));
	buf += sizeof(uint32_t);
	return val;
}

template <> string parse_field<string>(vector<char>::const_iterator& buf) {
	uint32_t size = parse_field<uint32_t>(buf);
	string str;
	str.insert(str.begin(), buf, buf + size);
	buf += size;
	return str;
}