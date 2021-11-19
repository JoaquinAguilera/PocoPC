#include <render.h>
#include <glew/glew.h>
#include <iostream>
#include <geometrybuffer.h>
#include <glm/ext/scalar_constants.hpp>
#include <device.h>

#include <texture.h>
#include <string>
#include <unordered_map>

namespace {

    void GLGetErrors() {
        while (GLenum error = glGetError())
        {
            std::cout << error << std::endl;
        }
    }

}

namespace poco::render::internal
{
    std::unordered_map<std::string, Texture*> texMap;


    unsigned int fbo;
    unsigned int fbo_texture;

    unsigned int shader;
    unsigned int shader2;

    unsigned int CompileShader(const std::string& shader, GLenum type);
    unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader);

    geometry::GeometryBuffer* geometryBuffers[4];

    std::string vertexShader =
        "#version 410 core\n"
        "layout(location = 0) in vec4 position;\n"
        "layout(location = 1) in vec3 colour;"
        "\n"
        "out vec3 v_colour;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "   v_colour = colour;\n"
        "}";


    std::string fragmentShader =
        "#version 410 core\n"
        "layout(location = 0) out vec4 colour;\n"
        "\n"
        "in vec3 v_colour;\n"
        "void main()\n"
        "{\n"
        "   colour = vec4(v_colour.xyz, 1.0);\n"
        "}";
    
    std::string vertexShaderSprite =
        "#version 410 core\n"
        "layout(location = 0) in vec4 position;\n"
        "layout(location = 1) in vec3 colour;"
        "layout(location = 2) in vec2 uv;"
        "\n"
        "out vec3 v_colour;\n"
        "out vec2 v_uv;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   gl_Position = position;\n"
        "   v_colour = colour;\n"
        "   v_uv = uv;\n"
        "}";


    std::string fragmentShaderSprite =
        "#version 410 core\n"
        "layout(location = 0) out vec4 colour;\n"
        "\n"
        "in vec3 v_colour;\n"
        "in vec2 v_uv;\n"
        "\n"
        "uniform sampler2D u_Texture;\n"
        "\n"
        "void main()\n"
        "{\n"
        "   vec4 texColour = texture(u_Texture, v_uv);\n"
        "   colour = texColour;\n"
        "}";
}

using namespace poco::render::internal;

void poco::render::init()
{
    GLGetErrors();
    //Generate frame buffer for low-res rendering
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &fbo_texture);
    glBindTexture(GL_TEXTURE_2D, fbo_texture);
    GLGetErrors();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_INT, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_texture, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        //assert
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    GLGetErrors();

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);


    //TO-DO Move Shaders into own class
    glViewport(0, 0, 128, 128);
    //Create Shaders
    shader = CreateShader(vertexShaderSprite, fragmentShaderSprite);
    shader2 = CreateShader(vertexShader, fragmentShader);

    GLGetErrors();
    //Set up Shader

    GLGetErrors();
    geometryBuffers[geometry::BufferType::Line] = new geometry::PrimitiveBuffer(shader2, GL_LINES);
    geometryBuffers[geometry::BufferType::Point] = new geometry::PrimitiveBuffer(shader2, GL_POINTS);
    geometryBuffers[geometry::BufferType::Triangle] = new geometry::PrimitiveBuffer(shader2, GL_TRIANGLES);
    geometryBuffers[geometry::BufferType::Sprite] = new geometry::SpriteBuffer(shader);
    GLGetErrors();

    GLGetErrors();
    //pixel(glm::vec2(40, 32));
    //line(glm::vec2(0, 0), glm::vec2(64, 64));
    //rect(glm::vec2(5, 20), glm::vec2(5, 20));
    //circle(glm::vec2(48, 16), 7);

    //glClearColor(0.39f, 0.58f, 0.93f, 1.0f); Cornflower blue
    glClearColor(0.513f, 0.462f, 0.611f, 1.0f);
    //glClearColor(0, 0, 0, 1.0);
}

unsigned int poco::render::internal::CompileShader(const std::string& shader, GLenum type)
{
    unsigned int id = glCreateShader(type);
    const char* src = shader.c_str();
    glShaderSource(id, 1, &src, nullptr);
    
    glCompileShader(id);

    //Check whether compile is successful
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "failed to compile " << ((type == GL_VERTEX_SHADER) ? "vertex" : "fragment") << " shader." << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

unsigned int poco::render::internal::CreateShader(const std::string& vertexShader,const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(vertexShader, GL_VERTEX_SHADER);
    unsigned int fs = CompileShader(fragmentShader, GL_FRAGMENT_SHADER);
    glAttachShader(program, fs);
    glAttachShader(program, vs);
    glLinkProgram(program);
    glValidateProgram(program);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}


void poco::render::shutdown()
{
    glDeleteProgram(shader);
}

void poco::render::render(float time)
{
    render::clear();

    //To-do Update this value on resize callback only.
    glm::vec2 dimensions = device::getWindowSize();

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    //Draw all buffers
    for (auto buffer : geometryBuffers)
    {
        buffer->Draw(time);
        buffer->Clear();
    }

    //Blit Framebuffer to Higher res render target
    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

    int renderSize = glm::min(dimensions.x, dimensions.y);

    //glBlitFramebuffer(0, 0, 128, 128, 0, 0, dimensions.x, dimensions.y, GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBlitFramebuffer(0, 0, 128, 128, (dimensions.x - renderSize) * 0.5f, (dimensions.y - renderSize) * 0.5f, ((dimensions.x - renderSize) * 0.5f) + renderSize, ((dimensions.y - renderSize) * 0.5f) + renderSize, GL_COLOR_BUFFER_BIT, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void poco::render::clear()
{
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glClear(GL_COLOR_BUFFER_BIT);
}


/////////////////
// Draw Functions
/////////////////

using namespace poco::geometry;

void poco::render::pixel(glm::vec2 pos, glm::vec3 colour)
{
    PrimitiveBuffer* pixelBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Point]);

    pixelBuffer->QueueVertex(pos, colour);
}

void poco::render::circle(glm::vec2 centre, int radius, glm::vec3 colour)
{
    PrimitiveBuffer* triangleBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Triangle]);

    int triCount = 20;
    float distance = (glm::pi<double>() * 2.0) / triCount;

    for (int i = 0; i < triCount; i++) {
        triangleBuffer->QueueVertex(centre, colour);
        glm::vec2 pos2 = centre + glm::vec2(glm::cos(distance * i) * radius,glm::sin(distance * i) * radius);
        glm::vec2 pos3 = centre + glm::vec2(glm::cos(distance * (i+1)) * radius,glm::sin(distance * (i+1)) * radius);
        triangleBuffer->QueueVertex(pos2, colour);
        triangleBuffer->QueueVertex(pos3, colour);

    }
}

void poco::render::oval(glm::vec2 topleft, glm::vec2 botright, glm::vec3 colour)
{
    int halfwidth = (botright.x - topleft.x) * 0.5f;
    int halfheight = (botright.y - topleft.y) * 0.5;

    glm::vec2 centre = glm::vec2(topleft.x + halfwidth, topleft.y + halfheight);

    PrimitiveBuffer* triangleBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Triangle]);

    int triCount = 10;
    float distance = (glm::pi<double>() * 2.0) / triCount;

    for (int i = 0; i < triCount; i++) {
        triangleBuffer->QueueVertex(centre, colour);
        glm::vec2 pos2 = centre + glm::vec2(glm::cos(distance * i) * halfwidth, glm::sin(distance * i) * halfheight);
        glm::vec2 pos3 = centre + glm::vec2(glm::cos(distance * (i + 1)) * halfwidth, glm::sin(distance * (i + 1)) * halfheight);
        triangleBuffer->QueueVertex(pos2, colour);
        triangleBuffer->QueueVertex(pos3, colour);

    }
}

void poco::render::circleOutline(glm::vec2 centre, int radius, glm::vec3 colour)
{
    PrimitiveBuffer* lineBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Line]);

    int triCount = 20;
    float distance = (glm::pi<double>() * 2.0) / triCount;

    for (int i = 0; i < triCount; i++) {
        glm::vec2 pos2 = centre + glm::vec2(glm::cos(distance * i) * radius, glm::sin(distance * i) * radius);
        glm::vec2 pos3 = centre + glm::vec2(glm::cos(distance * (i + 1)) * radius, glm::sin(distance * (i + 1)) * radius);
        lineBuffer->QueueVertex(pos2, colour);
        lineBuffer->QueueVertex(pos3, colour);
    }
}

void poco::render::line(glm::vec2 startPos, glm::vec2 endPos, glm::vec3 colour)
{
    PrimitiveBuffer* lineBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Line]);

    lineBuffer->QueueVertex(startPos, colour);
    lineBuffer->QueueVertex(endPos, colour);
}


void poco::render::rect(glm::vec2 origin, glm::vec2 dimensions, glm::vec3 colour)
{
    PrimitiveBuffer* triangleBuffer = dynamic_cast<PrimitiveBuffer*>(geometryBuffers[BufferType::Triangle]);

    triangleBuffer->QueueVertex(origin, colour);
    triangleBuffer->QueueVertex(origin + glm::vec2(dimensions.x, 0), colour);
    triangleBuffer->QueueVertex(origin + glm::vec2(0, dimensions.y), colour);

    triangleBuffer->QueueVertex(origin + glm::vec2(0, dimensions.y), colour);
    triangleBuffer->QueueVertex(origin + glm::vec2(dimensions.x, 0), colour);
    triangleBuffer->QueueVertex(origin + dimensions, colour);
}

void poco::render::sprite(glm::vec2 topleft, Texture* texture, glm::vec2 scale, bool flippedX, bool flippedY, glm::vec3 colour, glm::vec4 texCoords)
{
    SpriteBuffer* spriteBuffer = dynamic_cast<SpriteBuffer*>(geometryBuffers[BufferType::Sprite]);

    glm::vec2 dimensions = glm::vec2(texture->getWidth() * scale.x, texture->getHeight() * scale.y);
    spriteBuffer->QueueSprite(topleft, colour, dimensions, texture, texCoords, flippedX, flippedY);
}

void poco::render::sprite(glm::vec2 topleft, std::string path, glm::vec2 scale, bool flippedX, bool flippedY, glm::vec3 colour, glm::vec4 texCoords)
{
    std::unordered_map<std::string, Texture*>::const_iterator found = texMap.find(path);
    Texture* texture;

    if (found == texMap.end())
        texMap[path] = texture = new Texture(path);
    else
        texture = found->second;

    sprite(topleft, texture, scale, flippedX, flippedY, colour, texCoords);
}

void poco::render::spritesheet(SpriteSheet* sprites, int index, glm::vec2 topleft, glm::vec2 scale, bool flippedX, bool flippedY, glm::vec3 colour)
{
    int spritesPerRow = glm::ceil(sprites->getWidth() / sprites->getSpriteSize().x);

    int spriteX = index % spritesPerRow;
    int spriteY = glm::floor(index / spritesPerRow);

    float texwidth = (1.f / sprites->getWidth()) * sprites->getSpriteSize().x;
    float texheight = (1.f / sprites->getHeight()) * sprites->getSpriteSize().y;

    glm::vec2 topLeft = glm::vec2(texwidth * spriteX, texheight * spriteY);
    glm::vec2 bottomRight = topLeft + glm::vec2(texwidth, texheight);



    SpriteBuffer* spriteBuffer = dynamic_cast<SpriteBuffer*>(geometryBuffers[BufferType::Sprite]);
    spriteBuffer->QueueSprite(topleft, colour, sprites->getSpriteSize() * scale, sprites, glm::vec4(topLeft.x, topLeft.y, bottomRight.x, bottomRight.y), flippedX, flippedY);
}

int poco::render::readPixel(glm::vec2 pos) 
{
    float RGB[3];

    glBindFramebuffer(GL_READ_FRAMEBUFFER, fbo);
    glReadPixels(pos.x, 128-pos.y, 1, 1, GL_RGB, GL_FLOAT, &RGB);

    return 1;
}