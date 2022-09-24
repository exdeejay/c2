#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <cstdint>
#include <string>

using iter_t = std::vector<uint8_t>::const_iterator;


template<class T> struct Field {
	static T parse_field(iter_t& buf, const iter_t& end);
	static void serialize_field(const T& val, std::vector<uint8_t>& buf);
};

template<class T> struct Field<std::vector<T>> {
	static std::vector<T> parse_field(iter_t& buf, const iter_t& end) {
		uint32_t size = Field<uint32_t>::parse_field(buf, end);
		vector<T> data;
		for (size_t i = 0; i < size; i++) {
			data.push_back(Field<T>::parse_field(buf, end));
		}
		return data;
	}
	static void serialize_field(const std::vector<T>& val, std::vector<uint8_t>& buf) {
		Field<uint32_t>::serialize_field(val.size(), buf);
		for (const T& item : val) {
			Field<T>::serialize_field(item, buf);
		}
	}
};

#endif