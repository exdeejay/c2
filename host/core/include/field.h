#ifndef FIELD_H
#define FIELD_H

#include <vector>
#include <optional>
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
		std::vector<T> data;
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

template<class T> struct Field<std::optional<T>> {
	static std::optional<T> parse_field(iter_t& buf, const iter_t& end) {
		bool valid = Field<bool>::parse_field(buf, end);
		std::optional<T> data;
		if (valid) {
			data = Field<T>::parse_field(buf, end);
		}
		return data;
	}
	static void serialize_field(const std::optional<T>& val, std::vector<uint8_t>& buf) {
		Field<bool>::serialize_field(val.has_value(), buf);
		if (val.has_value()) {
			Field<T>::serialize_field(val, buf);
		}
	}
};

#define PARSE_ENUM(x) \
	template<> \
	x Field<x>::parse_field(iter_t& buf, const iter_t& end) { \
		return static_cast<x>(Field<uint8_t>::parse_field(buf, end)); \
	} \
	template<> \
	void Field<x>::serialize_field(const x& val, std::vector<uint8_t>& buf) { \
		Field<uint8_t>::serialize_field((uint8_t) val, buf); \
	}

#endif