#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fstream>
#include <memory>
#include <filesystem>
#include <vector>
#include <switch.h>

#include <Utility.hpp>

void printInfo()
{
    printf("BigCatAndTea give you a warm welcome!\n\n");

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


std::vector<NewsArchive> NewsManager::GetNewsFromRemote()
{
    Result rc = 0;
    NewsDatabaseService db;
    std::vector<NewsArchive> archives;
    
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
            
            /* Sanity check. */
            if (records_read != count) {
                printf("record count mismatch: %d vs %d\n", count, records_read);
                break;
            }

            for (auto &record : records) {
                NewsDataService data;
                if (R_FAILED(rc = newsCreateNewsDataService(&data)))
                    continue;

                do {
                    if (R_FAILED(rc = newsDataOpenWithNewsRecord(&data, &record)))
                        break;

                    u64 archiveSize = 0;
                    if (R_FAILED(rc = newsDataGetSize(&data, &archiveSize)))
                        break;

                    u64 bytes_read;
                    std::vector<u8> buffer(archiveSize);
                    if (R_FAILED(rc = newsDataRead(&data, &bytes_read, 0, buffer.data(), buffer.size())))
                        break;

                    archives.emplace_back(record.news_id, std::move(buffer));
                } while (false);

                if (R_FAILED(rc))
                    printf("failed to read record %s with 0x%x\n", record.news_id, rc);

                newsDataClose(&data);
            }
        } while (false);

        /* Close service. */
        newsDatabaseClose(&db);
    }

    if (R_FAILED(rc)) {
        printf("Failed to retreive remote news: 0x%x\n", rc);
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
        consoleUpdate(NULL);
    }

    consoleExit(NULL);
    newsExit();

    return 0;
}