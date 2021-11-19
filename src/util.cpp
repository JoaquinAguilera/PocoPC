#include <util.h>
#include <iostream>
#include <sstream>

using namespace poco;
namespace
{
    glm::vec4 colourmap[] =
    {
        glm::vec4(0, 0, 0, 0),//Transparent
        glm::vec4(0.113f, 0.168f, 0.325f, 1),//1
        glm::vec4(0.494f, 0.145f, 0.325f, 1),//2
        glm::vec4(0, 0.529f, 0.317f, 1),//3
        glm::vec4(0.670f, 0.321f, 0.211f, 1),//4
        glm::vec4(0.37f, 0.35f, 0.33f, 1),//5
        glm::vec4(0.760f, 0.764f, 0.780f, 1),//6
        glm::vec4(1, 0.945f, 0.909f, 1),//7
        glm::vec4(1.f, 0.f, 0.3f, 1),//8
        glm::vec4(1, 0.639f, 0, 1),//9
        glm::vec4(1, 0.925f, 0.152f, 1),//10
        glm::vec4(0, 0.894f, 0.211f, 1),//11
        glm::vec4(0.603f, 0.874f, 0.058f, 1),//12
        glm::vec4(0.215f, 0.411f, 0.047f, 1),//13
        glm::vec4(0.968f, 0.478f, 0.031f, 1),//14
        glm::vec4(0.988f, 0.792f, 0.039f, 1),//15
    };
}

std::vector<uint8_t> poco::util::hexToBytes(const std::string& hexString, bool swapPairs) 
{
    std::vector<uint8_t> byteVector;
    std::stringstream ss(hexString);
    
    while (ss.good())
    {
        char pair[3];
        ss.get(&pair[0], std::streamsize(3), '\n');

        if (swapPairs) {
            char temp = pair[1];
            pair[1] = pair[2];
            pair[2] = temp;
        }
        //Interpret pair of characters into hex value
        long num = std::strtol(pair, nullptr, 16);

        byteVector.push_back((uint8_t)num);
    }
    //Null terminator is also converted to a hex value
    //'\n' is already used for deliminator however so we just pop it
    byteVector.pop_back();
    return byteVector;
}

std::vector<uint8_t> poco::util::hexToNibbles(const std::string& hexString)
{
    std::vector<uint8_t> nibbleVector;
    std::stringstream ss(hexString);
    while (ss.good())
    {
        char pair[2];
        ss.get(&pair[0], std::streamsize(2), '\n');

        long num = std::strtol(pair, nullptr, 16);

        nibbleVector.push_back((uint8_t)num);
    }
    //Figure out that trailing 0 in a better way again
    nibbleVector.pop_back();
    return nibbleVector;
}

glm::vec4 poco::util::p8col(int i)
{
    if (i > 15) 
    {
        //invalid colour
        return glm::vec4(0, 0, 0, 0);
    }
    return colourmap[i];
}
