#include <cstdint>
#include "field.h"
#include "commands.h"

#define PARSE_ENUM(x) \
	template<> \
	x Field<x>::parse_field(iter_t& buf, const iter_t& end) { \
		return static_cast<x>(Field<uint8_t>::parse_field(buf, end)); \
	} \
	template<> \
	void Field<x>::serialize_field(const x& val, std::vector<uint8_t>& buf) { \
		Field<uint8_t>::serialize_field((uint8_t) val, buf); \
	}

PARSE_ENUM(NavigationCommandEnum)
PARSE_ENUM(DiscordCommandEnum)
PARSE_ENUM(AudioCommandEnum)
PARSE_ENUM(ShowoffCommandEnum)
