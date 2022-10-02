#include <vector>
#include <cstdint>
#include "field.h"
#include "datatypes.h"
using namespace std;

template<>
KeyEvent Field<KeyEvent>::parse_field(iter_t& buf, const iter_t& end) {
	KeyEvent keyEvent;
    keyEvent.keyDown = Field<bool>::parse_field(buf, end);
    keyEvent.vkCode = Field<uint32_t>::parse_field(buf, end);
    return keyEvent;
}

template<>
void Field<KeyEvent>::serialize_field(const KeyEvent& val, vector<uint8_t>& buf) {
    Field<bool>::serialize_field(val.keyDown, buf);
    Field<uint32_t>::serialize_field(val.vkCode, buf);
}
