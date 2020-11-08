#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <fstream>
#include <filesystem>

#include <switch.h>

extern "C" {
    extern u32 __start__;

    u32 __nx_applet_type = AppletType_None;

    #define INNER_HEAP_SIZE 0x18000
    size_t nx_inner_heap_size = INNER_HEAP_SIZE;
    char   nx_inner_heap[INNER_HEAP_SIZE];

    void __libnx_initheap(void);
    void __appInit(void);
    void __appExit(void);
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

    fsdevMountSdmc();
}

void userAppExit(void);

void __appExit(void)
{
    fsdevUnmountAll();
    fsExit();
    newsExit();
    smExit();
}

int main(int argc, char* argv[])
{

    std::error_code res;
    for (auto &&entry : std::filesystem::directory_iterator("sdmc:/News", res)) {
      if (entry.path().extension() != ".msgpack") {
          continue;
        }

      size_t file_size = entry.file_size();
      auto buffer = std::make_unique<char[]>(file_size);
      std::ifstream ifs(entry.path());
      ifs.read(buffer.get(), file_size);

      newsPostLocalNews(buffer.get(), file_size);

    }
}