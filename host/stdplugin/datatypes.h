#ifndef DATATYPES_H
#define DATATYPES_H

#include <cstdint>
#include <string>

struct FileData {
    std::string name;
    uint32_t attrs;
    uint64_t creation_time;
    uint64_t last_access_time;
    uint64_t last_write_time;
    uint64_t size;
};

#endif