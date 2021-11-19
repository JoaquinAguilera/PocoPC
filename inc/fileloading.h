#pragma once
#ifndef fileloading_h
#define fileloading_h
#include <string>
#include <vector>
#include <cartridge.h>

namespace poco::file
{
    void init();
    void shutdown();

    unsigned char* loadTexture(const std::string& image_file, int& Width, int& Length);
    std::vector<char> loadTextFile(const std::string& text_path);
}
#endif 