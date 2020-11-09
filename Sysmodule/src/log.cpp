/*
This code was taken from https://github.com/skyline-dev/eiffel/blob/master/source/util.cpp
*/

#include "log.hpp"

#ifdef NDEBUG
void __libnx_exception_handler(ThreadExceptionDump* ctx) {}
void debugInit() {}
void debugExit() {}

#else

#include <cstdarg>
#include <cstdio>
#include <string>

#ifdef DEBUG_LOG_FILE

FILE* g_debug_file;

extern "C" {
void __libnx_init_time(void);
}

#endif

#ifdef DEBUG_NXLINK_FORCED_IP

#include <netinet/in.h>

extern "C" {
extern struct in_addr __nxlink_host;
}

#endif

void debugInit() {
#ifdef DEBUG_LOG_FILE
    std::string logName = "sdmc:/eiffel" + std::to_string(currentTime) + ".log";  // TODO: better log location
    g_debug_file = fopen(logName.c_str(), "w");
    if (g_debug_file == NULL) fatalThrow(0xf);
#endif

#ifdef DEBUG_NXLINK_FORCED_IP
    __nxlink_host.s_addr = DEBUG_NXLINK_FORCED_IP;
#endif

#ifdef DEBUG_NXLINK
    nxlinkStdio();
#endif
}

void debugExit() {
#ifdef DEBUG_LOG_FILE
    fclose(g_debug_file);
#endif
}

void debugLogFormat(const char* prettyFunction, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);

#ifdef DEBUG_LOG_FILE
    if (prettyFunction) fprintf(g_debug_file, "%s: ", prettyFunction);
    vfprintf(g_debug_file, fmt, args);
    fflush(g_debug_file);
#endif

#ifdef DEBUG_NXLINK
    if (prettyFunction) printf("%s: ", prettyFunction);
    vprintf(fmt, args);
#endif

    va_end(args);
}

/*void __libnx_exception_handler(ThreadExceptionDump* ctx) {
    MemoryInfo mem_info;
    u32 page_info;
    svcQueryMemory(&mem_info, &page_info, ctx->pc.x);
    LOG("%#x exception with pc=%#lx", ctx->error_desc, ctx->pc.x - mem_info.addr);
    debugExit();
}*/

#endif
