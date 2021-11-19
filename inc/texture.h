#pragma once
#include <string>
#include <glm/glm.hpp>
class Texture
{
public:
    Texture(const std::string& path);
    Texture(const unsigned char const* texBuffer, int Width, int Height);
    ~Texture();

    void Bind();
    void Unbind();

    const inline int getWidth() { return m_Width; };
    const inline int getHeight() { return m_Height; };
private:
    unsigned int m_TexID;

    std::string m_FilePath;
    int m_Width = 0;
    int m_Height = 0;
    int m_BPP = 0;

    void Init(const unsigned char const* texBuffer);
};

class SpriteSheet : public Texture
{
public:
    SpriteSheet(std::string path) : Texture(path) {}
    SpriteSheet(const unsigned char const* texBuffer, int Width, int Height) : Texture(texBuffer, Width, Height) {}
    ~SpriteSheet() {}

    const inline glm::vec2 getSpriteSize() { return glm::vec2(m_SpriteWidth, m_SpriteHeight); };
    inline void setSpriteSize(glm::vec2 size) { m_SpriteWidth = size.x; m_SpriteHeight = size.y; };
private:
    int m_SpriteWidth = 8;
    int m_SpriteHeight = 8;
};