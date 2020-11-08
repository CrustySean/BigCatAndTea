#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory>
#include <fstream>
#include <filesystem>

#include <switch.h>


u32 __nx_applet_type = AppletType_None;

u32 __nx_fs_num_sessions = 1;

#define INNER_HEAP_SIZE 0x80000
size_t nx_inner_heap_size = INNER_HEAP_SIZE;
char   nx_inner_heap[INNER_HEAP_SIZE];

void __libnx_initheap(void)
{
	void*  addr = nx_inner_heap;
	size_t size = nx_inner_heap_size;

	extern char* fake_heap_start;
	extern char* fake_heap_end;

	fake_heap_start = (char*)addr;
	fake_heap_end   = (char*)addr + size;
}

void __attribute__((weak)) __appInit(void)
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

void __attribute__((weak)) userAppExit(void);

void __attribute__((weak)) __appExit(void)
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