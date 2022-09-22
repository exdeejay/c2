#include "field.h"
#include <Windows.h>
#include <iostream>
using namespace std;

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

template<> bool parse_field<bool>(iter_t& buf, const iter_t& end) {
	return parse_field<char>(buf, end) != 0;
}
template<> void serialize_field<bool>(const bool& val, std::vector<char>& buf) {
	serialize_field<char>((char) val, buf);
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
	string str(buf, buf + size);
	buf += size;
	return str;
}
template<> void serialize_field<string>(const string& val, vector<char>& buf) {
	serialize_field<uint32_t>(val.size(), buf);
	for (char c : val) {
		buf.push_back(c);
	}
}

template<>
vector<char> parse_field<vector<char>>(iter_t& buf, const iter_t& end) {
	uint32_t size = parse_field<uint32_t>(buf, end);
	vector<char> data(buf, buf + size);
	buf += size;
	return data;
}
template<>
void serialize_field<std::vector<char>>(const vector<char>& val, vector<char>& buf) {
	serialize_field<uint32_t>(val.size(), buf);
	for (char c : val) {
		buf.push_back(c);
	}
}



template<>
DiscordCommand parse_field<DiscordCommand>(iter_t& buf, const iter_t& end) {
	return static_cast<DiscordCommand>(parse_field<unsigned char>(buf, end));
}
template<>
void serialize_field<DiscordCommand>(const DiscordCommand& val, vector<char>& buf) {
	serialize_field<unsigned char>((unsigned char) val, buf);
}

template<>
AudioCommand parse_field<AudioCommand>(iter_t& buf, const iter_t& end) {
	return static_cast<AudioCommand>(parse_field<unsigned char>(buf, end));
}
template<>
void serialize_field<AudioCommand>(const AudioCommand& val, vector<char>& buf) {
	serialize_field<unsigned char>((unsigned char) val, buf);
}
