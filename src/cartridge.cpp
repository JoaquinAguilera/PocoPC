#include "cartridge.h"
#include "util.h"
#include "fileloading.h"
#include <iostream>
#include <sstream>
using namespace poco;


poco::Cartridge::Cartridge(std::string path)
{
    //Load cartridge from text file
    std::vector<char> cartridgeData = file::loadTextFile(path);

    /*
    Attempting to set the streambuffer pointer to the existing cartridge doesn't work because pubsetbuf does nothing
    One implementation of this function would parse the separate parts of this file through referencing their respective
    locations and sizes within the buffer but instead i'm copying data because I don't want to write my own stringstream implementation right now
    std::stringbuf* streambuffer = stream.rdbuf();
    streambuffer->pubsetbuf(&cartridgeData[0], cartridgeData.size());
    */

    //Set stream's buffer to be cartridgeData
    std::stringstream stream;
    std::copy(cartridgeData.begin(), cartridgeData.end(), std::ostream_iterator<char>(stream));


    //Parse Cartridge info (title, author, etc.)
    std::string cartridgeInfo = LoadP8Section(stream, "\n");

    std::string sectionLabel;
    while (stream.good() && std::getline(stream, sectionLabel))
    {
        if (sectionLabel == "__lua__")
        {
            code = LoadP8Section(stream, "\n");
            //Set Lua in Cartridge
        }
        else if (sectionLabel == "__gfx__")
        {
            std::string spritesheetdata = LoadP8Section(stream);
            std::vector<uint8_t> colourIndexVec = util::hexToNibbles(spritesheetdata);

            unsigned char* textureBuffer = new unsigned char[colourIndexVec.size() * 4];
            for (int i = 0; i < colourIndexVec.size(); i++)
            {
                glm::vec4 col = util::p8col(colourIndexVec.at(i));
                unsigned char r = col.x * 255;
                unsigned char g = col.y * 255;
                unsigned char b = col.z * 255;
                unsigned char a = col.w * 255;
                textureBuffer[i * 4] = r;
                textureBuffer[i * 4 + 1] = g;
                textureBuffer[i * 4 + 2] = b;
                textureBuffer[i * 4 + 3] = a;
            }
            //m_Spritesheet = new SpriteSheet("celeste.png");
            m_Spritesheet = new SpriteSheet(textureBuffer, 128, 128);
            delete textureBuffer;
        }
        else if (sectionLabel == "__gff__")
        {
            std::string flags = LoadP8Section(stream);
            m_SpriteFlags = util::hexToBytes(flags);
        }
        else if (sectionLabel == "__map__")
        {
            std::string mapdata = LoadP8Section(stream);
            std::vector<uint8_t> mapvec = util::hexToBytes(mapdata, false);
            for (unsigned int i = 0; i < 128; i++)
            {
                for (unsigned int j = 0; j < 128; j++)
                {
                    if (mapvec.size() > (i * 128) + j)
                        m_Map[j][i] = mapvec.at((i * 128) + j);
                }
            }
        }
        else if (sectionLabel == "__label__")
        {
            LoadP8Section(stream);
        }
        else if (sectionLabel == "__music__")
        {
            LoadP8Section(stream);
        }
        else if (sectionLabel == "__sfx__")
        {
            LoadP8Section(stream);
        }
        else
        {
            LoadP8Section(stream);
        }

    }

}

poco::Cartridge::~Cartridge() 
{

};

std::string poco::Cartridge::LoadP8Section(std::stringstream& stream, std::string lineSeparator)
{
    std::stringstream buffer;
    std::string line;
    auto streamPos = stream.tellg();

    while (stream.good() && std::getline(stream, line))
    {
        if (line.find("__") != std::string::npos)
        {
            //Move back one line
            stream.seekg(streamPos);
            break;
        }
        streamPos = stream.tellg();
        buffer << line << lineSeparator;
    }
    return buffer.str();
}

uint8_t poco::Cartridge::ReadMap(unsigned int x, unsigned int y)
{
    uint8_t result = m_Map[x][y];
    return result;
}

bool poco::Cartridge::ReadFlag(unsigned int sprite, unsigned int flag) 
{
    if (sprite > m_SpriteFlags.size() || flag > 7)
        return false;

    int flagmask = pow(2, flag);
    return flagmask & m_SpriteFlags.at(sprite);
}

uint8_t poco::Cartridge::ReadFlag(unsigned int sprite)
{
    return sprite < m_SpriteFlags.size() ? m_SpriteFlags.at(sprite) : 0;
}