#pragma once
#include <vector>
#include <string>
#include <string>

#include "glm/vec4.hpp"

namespace poco::util 
{
    std::vector<uint8_t> hexToBytes(const std::string& hexString, bool swapPairs = false);
    std::vector<uint8_t> hexToNibbles(const std::string& hexString);

    glm::vec4 p8col(int index);
}