#pragma once
#include "texture.h"
#include <vector>
#include <string>
#include <sstream>

namespace poco
{

    class Cartridge {
    public:
        Cartridge(std::string path);
        ~Cartridge();

        uint8_t ReadMap(unsigned int x, unsigned int y);
        bool ReadFlag(unsigned int sprite, unsigned int flag);
        uint8_t ReadFlag(unsigned int sprite);
        SpriteSheet* m_Spritesheet;
        std::string code;
    private:
        std::string LoadP8Section(std::stringstream& stream, std::string lineSeparator = "");


        std::string m_path;
        uint8_t m_Map[128][128] = {0};
        std::vector<uint8_t> m_SpriteFlags;
    };
}