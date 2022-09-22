#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <cstdint>
#include <string>
#include "commands.h"

using iter_t = std::vector<char>::const_iterator;

uint32_t byteswap32(uint32_t val);

template<class T> T parse_field(iter_t& buf, const iter_t& end);
template<class T> void serialize_field(const T& value_entA, std::vector<char>& buf);

template<> unsigned char parse_field<unsigned char>(iter_t& buf, const iter_t& end);
template<> void serialize_field<unsigned char>(const unsigned char& val, std::vector<char>& buf);
template<> char parse_field<char>(iter_t& buf, const iter_t& end);
template<> void serialize_field<char>(const char& val, std::vector<char>& buf);
template<> bool parse_field<bool>(iter_t& buf, const iter_t& end);
template<> void serialize_field<bool>(const bool& val, std::vector<char>& buf);
template<> uint32_t parse_field<uint32_t>(iter_t& buf, const iter_t& end);
template<> void serialize_field<uint32_t>(const uint32_t& val, std::vector<char>& buf);
template<> std::string parse_field<std::string>(iter_t& buf, const iter_t& end);
template<> void serialize_field<std::string>(const std::string& val, std::vector<char>& buf);
template<> std::vector<char> parse_field<std::vector<char>>(iter_t& buf, const iter_t& end);
template<> void serialize_field<std::vector<char>>(const std::vector<char>& val, std::vector<char>& buf);

enum class DiscordCommand;
template<> DiscordCommand parse_field<DiscordCommand>(iter_t& buf, const iter_t& end);
template<> void serialize_field<DiscordCommand>(const DiscordCommand& val, std::vector<char>& buf);
enum class AudioCommand;
template<> AudioCommand parse_field<AudioCommand>(iter_t& buf, const iter_t& end);
template<> void serialize_field<AudioCommand>(const AudioCommand& val, std::vector<char>& buf);

#endif