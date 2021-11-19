#include <texture.h>
#include <fileloading.h>
#include <glew/glew.h>
#include <iostream>
#include <stb/stb_image.h>

namespace {

    void GLGetErrors() {
        while (GLenum error = glGetError())
        {
            #if defined (PLATFORM_PC)
            std::cout << error << std::endl;
            #endif
        }
    }
}

Texture::Texture(const unsigned char const* texBuffer, int Width, int Height)
    : m_Width(Width), m_Height(Height)
{
    Texture::Init(texBuffer);
}

Texture::Texture(const std::string& path)
{
    unsigned char* texBuffer = poco::file::loadTexture(path, m_Width, m_Height);// stbi_load(path.c_str(), &m_Width, &m_Height, &word, 4);
    Texture::Init(texBuffer);
    if (texBuffer != nullptr)
    {
        stbi_image_free(texBuffer);
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_TexID);
}

void Texture::Init(const unsigned char const* texBuffer)
{
    //With inverted coordinate system this is no longer needed
    //stbi_set_flip_vertically_on_load(1);
    GLGetErrors();
    glGenTextures(1, &m_TexID);
    glBindTexture(GL_TEXTURE_2D, m_TexID);

    GLGetErrors();

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    GLGetErrors();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texBuffer);

    GLGetErrors();

    glBindTexture(GL_TEXTURE_2D, 0);

    GLGetErrors();

}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TexID);
    GLGetErrors();
}

void Texture::Unbind()
{
    GLGetErrors();
}