#pragma once
#include <glm/glm.hpp>
#include <string>
#include "texture.h"

namespace poco::render
{
    void init();
    void shutdown();
    void render(float time);
    void clear();

    //Draw Functions

    void pixel(glm::vec2 pos, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void circle(glm::vec2 centre, int radius, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void circleOutline(glm::vec2 centre, int radius, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void line(glm::vec2 startPos, glm::vec2 endPos, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void rect(glm::vec2 origin, glm::vec2 dimensions, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void sprite(glm::vec2 topleft, Texture* texture, glm::vec2 scale = glm::vec2(1,1), bool flippedX = false, bool flippedY = false, glm::vec3 colour = glm::vec3(-1, -1, -1), glm::vec4 texCoords = glm::vec4(1.f,1.f,0.f,0.f));
    void sprite(glm::vec2 topleft, std::string path, glm::vec2 scale = glm::vec2(1,1), bool flippedX = false, bool flippedY = false, glm::vec3 colour = glm::vec3(-1, -1, -1), glm::vec4 texCoords = glm::vec4(1.f, 1.f, 0.f, 0.f));
    void spritesheet(SpriteSheet* sprites, int index, glm::vec2 topleft, glm::vec2 scale = glm::vec2(1,1), bool flippedX = false, bool flippedY = false, glm::vec3 colour = glm::vec3(-1, -1, -1));
    void oval(glm::vec2 topleft, glm::vec2 botright, glm::vec3 colour = glm::vec3(-1, -1, -1));
    //void text() {};


    int readPixel(glm::vec2 pos);
}