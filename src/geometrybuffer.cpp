#include <geometrybuffer.h>
#include <glew/glew.h>
#include <cstdlib>
#include <ctime>

#include <iostream>

using namespace poco::geometry;

namespace poco::geometry::internal {

    void PixelToScreenCoords(glm::vec2 &pos) 
    {
        int screensize = 128;
        
        float screenX = (((1.f / screensize) * pos.x) - 0.5f) *  2.f;
        float screenY = -(((1.f / screensize) * pos.y) - 0.5f) * 2.f;
        
        pos = glm::vec2(screenX, screenY);
    }
}

using namespace poco::geometry::internal;

GeometryBuffer::GeometryBuffer(unsigned int ShaderID) : m_shaderID(ShaderID) {
    //Generate vertex array object
    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    //Generate vertex buffer object
    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
}

GeometryBuffer::~GeometryBuffer() {


}

PrimitiveBuffer::PrimitiveBuffer(unsigned int ShaderID, unsigned int primitiveType) : GeometryBuffer::GeometryBuffer(ShaderID) {
    m_primitiveType = primitiveType;

    //Set buffer layout
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



void PrimitiveBuffer::QueueVertex(glm::vec2 position, glm::vec3 colour) {
    PixelToScreenCoords(position);
    m_Vertices[m_VertexCount * 5 + 0] = position.x;
    m_Vertices[m_VertexCount * 5 + 1] = position.y;

    m_Vertices[m_VertexCount * 5 + 2] = colour.x;
    m_Vertices[m_VertexCount * 5 + 3] = colour.y;
    m_Vertices[m_VertexCount * 5 + 4] = colour.z;

    m_VertexCount++;
}

void PrimitiveBuffer::Draw(float dt) {
    glUseProgram(m_shaderID);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_VertexCount * 5 * sizeof(float), &m_Vertices[0], GL_DYNAMIC_DRAW);

    glDrawArrays(m_primitiveType, 0, m_VertexCount);
}


SpriteBuffer::SpriteBuffer(unsigned int ShaderID) : GeometryBuffer::GeometryBuffer(ShaderID) {
    std::srand(std::time(nullptr));
    //Set buffer layout
    //position attribute
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    //color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //texcoords attributekij
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void SpriteBuffer::QueueSpriteVertex(glm::vec2 pos, glm::vec3 col, glm::vec2 uv) {
    PixelToScreenCoords(pos);
    col = glm::vec3(static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX), static_cast <float> (rand()) / static_cast <float> (RAND_MAX));
    m_Vertices[m_VertexCount * 7 + 0] = pos.x;
    m_Vertices[m_VertexCount * 7 + 1] = pos.y;

    m_Vertices[m_VertexCount * 7 + 2] = col.x;
    m_Vertices[m_VertexCount * 7 + 3] = col.y;
    m_Vertices[m_VertexCount * 7 + 4] = col.z;

    m_Vertices[m_VertexCount * 7 + 5] = uv.x;
    m_Vertices[m_VertexCount * 7 + 6] = uv.y;

    m_VertexCount++;

}

void SpriteBuffer::QueueSprite(glm::vec2 position, glm::vec3 colour, glm::vec2 dimensions, Texture* texture, glm::vec4 texCoords, bool flippedX, bool flippedY) {
    QueueSpriteVertex(position, colour, glm::vec2(flippedX ? texCoords.z : texCoords.x, flippedY ? texCoords.w : texCoords.y)); //TopLeft
    QueueSpriteVertex(position + glm::vec2(dimensions.x,0), colour, glm::vec2(flippedX ? texCoords.x : texCoords.z, flippedY ? texCoords.w : texCoords.y)); // TopRight
    QueueSpriteVertex(position + glm::vec2(0,dimensions.y), colour, glm::vec2(flippedX ? texCoords.z : texCoords.x, flippedY ? texCoords.y : texCoords.w)); //BottomLeft

    QueueSpriteVertex(position + glm::vec2(0, dimensions.y), colour, glm::vec2(flippedX ? texCoords.z : texCoords.x, flippedY ? texCoords.y : texCoords.w)); //BottomLeft
    QueueSpriteVertex(position + glm::vec2(dimensions.x, 0), colour, glm::vec2(flippedX ? texCoords.x : texCoords.z, flippedY ? texCoords.w : texCoords.y)); // TopRight
    QueueSpriteVertex(position + dimensions, colour, glm::vec2(flippedX ? texCoords.x : texCoords.z, flippedY ? texCoords.y : texCoords.w)); //BottomRight

    textures[(m_VertexCount / 6) - 1] = texture;
}

void SpriteBuffer::Draw(float dt) {

    glUseProgram(m_shaderID);
    glUniform1i(glGetUniformLocation(m_shaderID, "u_Texture"), 0);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glBufferData(GL_ARRAY_BUFFER, m_VertexCount * 7 * sizeof(float), &m_Vertices[0], GL_DYNAMIC_DRAW);

    int spriteCount = (m_VertexCount / 6);


    for (int i = 0; i < spriteCount; i++) {
        textures[i]->Bind();
        glDrawArrays(GL_TRIANGLES, i * 6, 6);
    }
}