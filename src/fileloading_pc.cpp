#include <fileloading.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <fstream>
#include <sstream>


void poco::file::init() {};

void poco::file::shutdown() {};

unsigned char* poco::file::loadTexture(const std::string& image_file, int& Width, int& Length) 
{
    std::string FilePath = "resources/" + image_file;
    int BPP;

    return stbi_load(FilePath.c_str(), &Width, &Length, &BPP, 4);
};

std::vector<char> poco::file::loadTextFile(const std::string& text_path)
{
    //Open File
    std::string FilePath = "resources/" + text_path;
    std::ifstream file;
    file.open(FilePath);
    //If file is not valid assert return nothing
    assert(file);
    //Create buffer size of file
    file.seekg(0, std::ios::end);
    auto fileLength = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(fileLength);

    //Stream entire file into buffer
    file.read(&buffer[0], fileLength);
    //Return buffer
    file.close();
    return buffer;
};