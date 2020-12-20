#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <fstream>
#include <filesystem>

#include <switch.h>

#include "log.hpp"

#define DEBUG_LOG_FILE

extern "C" {
extern u32 __start__;

u32 __nx_applet_type = AppletType_None;
// u32 __nx_fs_num_sessions = 1;

#define INNER_HEAP_SIZE 0x1000000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void);
void __appInit(void);
void __appExit(void);
}

void __libnx_initheap(void) {
    extern char* fake_heap_start;
    extern char* fake_heap_end;
    fake_heap_start = nx_inner_heap;
    fake_heap_end = nx_inner_heap + sizeof(nx_inner_heap);
}


void __appInit(void)
{
    Result rc;

    rc = smInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));
    
    rc = newsInitialize(NewsServiceType_Administrator);
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_SM));

    rc = fsInitialize();
    if (R_FAILED(rc))
        fatalThrow(MAKERESULT(Module_Libnx, LibnxError_InitFail_FS));

    auto socketConfig = SocketInitConfig{.bsdsockets_version = 1,
                                             .tcp_tx_buf_size = 0x800,
                                             .tcp_rx_buf_size = 0x1000,
                                             .tcp_tx_buf_max_size = 0,
                                             .tcp_rx_buf_max_size = 0,
                                             .udp_tx_buf_size = 0x2400,
                                             .udp_rx_buf_size = 0xA500,
                                             .sb_efficiency = 1};

    fsdevMountSdmc();

    debugInit();
}

void userAppExit(void);

void __appExit(void)
{
    debugExit();
    fsdevUnmountAll();
    fsExit();
    newsExit();
    smExit();
    socketExit();
}

int main(int argc, char* argv[])
{
    LOG("main start");

    std::error_code res;
    for (auto &&entry : std::filesystem::directory_iterator("sdmc:/News", res)) {
      if (entry.path().extension() != ".msgpack") {
          continue;
        }

      size_t file_size = entry.file_size();
      auto buffer = std::make_unique<char[]>(file_size);
      std::ifstream ifs(entry.path());
      ifs.read(buffer.get(), file_size);

      Result rc = newsPostLocalNews(buffer.get(), file_size);
      if (R_FAILED(rc))
      LOG("Failed to post the news");

    }

    LOG("main exit");
    return 0;
}