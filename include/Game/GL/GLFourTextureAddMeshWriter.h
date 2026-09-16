#ifndef GAME_GL_GL_FOUR_TEXTURE_ADD_MESH_WRITER_H
#define GAME_GL_GL_FOUR_TEXTURE_ADD_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLFourTextureAddMeshWriter
{
public:
    GLFourTextureAddMeshWriter();
    ~GLFourTextureAddMeshWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    void Colour(const nlColour& colour)
    {
        *colours++ = *(const u32*)&colour;
    }

    void Texcoord0(nlVector2 value)
    {
        *texcoords0++ = value.x;
        *texcoords0++ = value.y;
    }

    void Texcoord1(nlVector2 value)
    {
        *texcoords1++ = value.x;
        *texcoords1++ = value.y;
    }

    void Texcoord2(nlVector2 value)
    {
        *texcoords2++ = value.x;
        *texcoords2++ = value.y;
    }

    void Texcoord3(nlVector2 value)
    {
        *texcoords3++ = value.x;
        *texcoords3++ = value.y;
    }

    void Vertex(float x, float y, float z)
    {
        *positions++ = x;
        *positions++ = y;
        *positions++ = z;
    }

    void Texture(int index, u32 texture)
    {
        glTextureBinding* binding
            = (glTextureBinding*)model->packets->materialParameters + index;
        binding->texture = texture;
        binding->textureIndex = 0xFFFF;
        binding->SetWrapS(true);
        binding->SetWrapT(true);
        binding->unknown07 = 0;
    }

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* positions;
    /* 0x10 */ float* texcoords0;
    /* 0x14 */ float* texcoords1;
    /* 0x18 */ float* texcoords2;
    /* 0x1C */ float* texcoords3;
    /* 0x20 */ u32* colours;
};

#endif // GAME_GL_GL_FOUR_TEXTURE_ADD_MESH_WRITER_H
