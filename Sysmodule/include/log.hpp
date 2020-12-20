/*
This code was taken from https://github.com/skyline-dev/eiffel/blob/master/source/util.hpp
*/

#pragma once

#include <switch.h>

#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)

// debug logging
// #define DEBUG_LOG_FILE
// #define DEBUG_NXLINK
// #define DEBUG_NXLINK_FORCED_IP 0x4500a8c0  // example: c0.a8.00.45 = 192.168.0.69

#ifdef NDEBUG
#define LOG(...) ({})
#else
void debugLogFormat(const char* prettyFunction, const char* fmt, ...);
#define LOG(fmt, ...) debugLogFormat(__PRETTY_FUNCTION__, fmt "\n" __VA_OPT__(, ) __VA_ARGS__)
#endif

#define LOG32(v) LOG(STRINGIFY(v) " = %d", v)
#define LOG64(v) LOG(STRINGIFY(v) " = %ld", v)
#define LOG32X(v) LOG(STRINGIFY(v) " = 0x%x", v)
#define LOG64X(v) LOG(STRINGIFY(v) " = 0x%lx", v)

void debugInit();
void debugExit();