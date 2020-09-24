#pragma once

#include <stdio.h>
#include <fstream>
#include <string.h>
#include <dirent.h>
#include <vector>

#include <switch.h>

class NewsArchive
{
private:
    std::string m_name;
    std::vector<u8> m_data;
public:
    NewsArchive(std::string name, std::vector<u8> &&data);
    Result Install(NewsArchive* archive);

};
