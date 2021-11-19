#pragma once
#pragma once
#ifndef luascripting_h
#define luascripting_h

#include <string>

namespace poco::lua
{
    void init(std::string source);
    void update();
    void shutdown();
}

#endif