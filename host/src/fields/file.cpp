#include <string>
#include <vector>
#include "field.h"
#include "datatypes.h"
using namespace std;

template<>
FileData Field<FileData>::parse_field(iter_t& buf, const iter_t& end) {
	FileData fileData;
    fileData.name = Field<string>::parse_field(buf, end);
    fileData.attrs = Field<uint32_t>::parse_field(buf, end);
    fileData.creation_time = Field<uint64_t>::parse_field(buf, end);
    fileData.last_access_time = Field<uint64_t>::parse_field(buf, end);
    fileData.last_write_time = Field<uint64_t>::parse_field(buf, end);
    fileData.size = Field<uint64_t>::parse_field(buf, end);
    return fileData;
}

template<>
void Field<FileData>::serialize_field(const FileData& val, vector<uint8_t>& buf) {
    Field<string>::serialize_field(val.name, buf);
    Field<uint32_t>::serialize_field(val.attrs, buf);
    Field<uint64_t>::serialize_field(val.creation_time, buf);
    Field<uint64_t>::serialize_field(val.last_access_time, buf);
    Field<uint64_t>::serialize_field(val.last_write_time, buf);
    Field<uint64_t>::serialize_field(val.size, buf);
}
