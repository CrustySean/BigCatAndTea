#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <memory>
#include <filesystem>
#include <vector>
#include <switch.h>

void printInfo()
{
    printf("BigCatAndTea give you a warm welcome!\n\n");

    printf("Y : Install BCAT News (from sdmc:/bcat)\n");
    printf("X : Backup BCAT news\n");
    printf("+ : Exit\n");
}

void BCATinstaller()
{
    std::error_code res;
    for (auto &&entry : std::filesystem::directory_iterator("sdmc:/bcat", res)) {
    if (entry.path().extension() != ".msgpack") {
        printf("skipping: %s\n", entry.path().filename().c_str());
        continue;
    }

    printf("posting: %s\n", entry.path().filename().c_str());

    size_t file_size = entry.file_size();
    auto buffer = std::make_unique<char[]>(file_size);
    std::ifstream ifs(entry.path());
    ifs.read(buffer.get(), file_size);

    Result rc = newsPostLocalNews(buffer.get(), file_size);
    if (R_FAILED(rc)) {
        printf("Failed to post the news\n");
    } else {
        printf("post: 0x%x\n", rc);

    };

  }
}

void BackupBCAT()
{
    Result rc = 0;
    NewsDatabaseService db;
    std::vector<BackupBCAT> archives;
    /* Open database service. */
    if (R_SUCCEEDED(rc = newsCreateNewsDatabaseService(&db))) {
        do {
            /* Count entries. */
            u32 count = 0;
            if (R_FAILED(rc = newsDatabaseCount(&db, "", &count)))
                break;

            /* Read records. */
            u32 records_read = 0;
            std::vector<NewsRecord> records(count);
            if (R_FAILED(rc = newsDatabaseGetList(&db, records.data(), count, "", "", &records_read, 0)))
                break;
            } while (false);

        /* Close service. */
        newsDatabaseClose(&db);
    }

    if (R_FAILED(rc)) {
        printf("Failed to open BCAT\n", rc);
    } else {
        printf("Successfully opened BCAT\n", rc);
    }

    return archives;

}

int main(int argc, char **argv)
{
    consoleInit(NULL);

    Result rc = newsInitialize(NewsServiceType_Administrator);
    if (R_FAILED(rc)) {
        printf("Failed to initialize news service\n");
    } else {
        printf("Successfully initialized news service\n");
    };
    
    printInfo();

    while(appletMainLoop())
    {
        hidScanInput();
        u64 kDown = hidKeysDown(CONTROLLER_P1_AUTO);

        if (kDown & KEY_PLUS) break;
        if (kDown & KEY_Y) BCATinstaller();
        if (kDown & KEY_X) BackupBCAT();
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    newsExit();

    return 0;
}