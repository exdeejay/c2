#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <cstdint>
#include <string>

using iter_t = std::vector<uint8_t>::const_iterator;

uint32_t byteswap32(uint32_t val);


template<class T> class Field {
public:
	static T parse_field(iter_t& buf, const iter_t& end);
	static void serialize_field(const T& val, std::vector<uint8_t>& buf);
};

template<class T> class Field<std::vector<T>> {
public:
	static std::vector<T> parse_field(iter_t& buf, const iter_t& end) {
		uint32_t size = Field<uint32_t>::parse_field(buf, end);
		vector<T> data;
		for (size_t i = 0; i < size; i++) {
			data.push_back(Field<T>::parse_field(buf, end));
		}
		return data;
	}
	static void serialize_field(const std::vector<T>& val, std::vector<char>& buf) {
		Field<uint32_t>::serialize_field(val.size(), buf);
		for (const T& item : val) {
			Field<T>::serialize_field(item, buf);
		}
	}
};


#define PARSE_ENUM(x) \
	enum class x; \
	template<> class Field<x> { \
		public: \
			static x parse_field(iter_t& buf, const iter_t& end) { \
				return static_cast<x>(Field<uint8_t>::parse_field(buf, end)); \
			} \
			static void serialize_field(const x& val, std::vector<uint8_t>& buf) { \
				Field<uint8_t>::serialize_field((uint8_t) val, buf); \
			} \
	};

PARSE_ENUM(DiscordCommand)
PARSE_ENUM(AudioCommand)
PARSE_ENUM(ShowoffCommand)


#endif