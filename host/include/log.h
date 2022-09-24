#ifndef LOG_H
#define LOG_H

#ifdef DEBUG

#include <cstdio>

#define DEBUGLOG(...) printf(__VA_ARGS__)

#else

#define DEBUGLOG(...)

#endif

#endif