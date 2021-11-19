#pragma once
#include <glm/glm.hpp>

namespace poco::device {
    void init();
    void shutdown();

    void swapbuffers();
    bool shouldClose();

    glm::vec2 getWindowSize();


}