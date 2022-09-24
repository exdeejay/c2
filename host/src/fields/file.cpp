#include "field.h"

// template<>
// bool Field<bool>::parse_field(iter_t& buf, const iter_t& end) {
// 	return Field<uint8_t>::parse_field(buf, end) != 0;
// }
// template<>
// void Field<bool>::serialize_field(const bool& val, vector<uint8_t>& buf) {
// 	Field<uint8_t>::serialize_field((uint8_t) val, buf);
// }