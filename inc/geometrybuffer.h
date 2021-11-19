#pragma once
#include <glm/glm.hpp>
#include <texture.h>

namespace poco::geometry {

    enum BufferType {
        Triangle,
        Point,
        Line,
        Sprite,
    };


    class GeometryBuffer {
    public:
        GeometryBuffer(unsigned int ShaderID);
        ~GeometryBuffer();

        inline void SetShader(unsigned int ShaderID) { m_shaderID = ShaderID; }
        void Clear() { m_VertexCount = 0; }
        virtual void Draw(float dt) = 0;

    protected:

        unsigned int m_shaderID;

        unsigned int m_VAO;
        unsigned int m_VBO;

        float m_Vertices[5120 * 5 * 3] = {}; //5120 Triangles
        unsigned int m_VertexCount = 0;
    };

    class PrimitiveBuffer : public GeometryBuffer {
    public:
        PrimitiveBuffer(unsigned int ShaderID, unsigned int primitiveType);
        void QueueVertex(glm::vec2 position, glm::vec3 colour);
        void Draw(float dt) override;
    private:
        unsigned int m_primitiveType;
    };

    class SpriteBuffer : public GeometryBuffer {
    public:
        SpriteBuffer(unsigned int ShaderID);
        void QueueSprite(glm::vec2 position, glm::vec3 colour, glm::vec2 dimensions, Texture* texture, glm::vec4 texCoords, bool flippedX = false, bool flippedY = false);
        void Draw(float dt) override;
    private:
        void QueueSpriteVertex(glm::vec2 pos, glm::vec3 col, glm::vec2 uv);

    private: 
        Texture* textures[2048] = {};//image Images[]

    };
}
