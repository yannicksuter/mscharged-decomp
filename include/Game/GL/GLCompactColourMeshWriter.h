#ifndef GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H
#define GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLCompactColourMeshWriter
{
public:
    GLCompactColourMeshWriter();
    ~GLCompactColourMeshWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        ColourPlat(*(const unsigned long*)&c);
    }

    void ColourPlat(unsigned long nColourPlat)
    {
        *colour++ = nColourPlat;
    }

    void Texcoord(const nlVector2& uv)
    {
        Texcoord(uv.x, uv.y);
    }

    void Texcoord(float u, float v)
    {
        short su = (short)(u * 1024.0f);
        *texcoord++ = su;
        short sv = (short)(v * 1024.0f);
        *texcoord++ = sv;
    }

    void Texcoord(short u, short v)
    {
        *texcoord++ = u;
        *texcoord++ = v;
    }

    void Vertex(const nlVector3& pos)
    {
        Vertex(pos.x, pos.y, pos.z);
    }

    void Vertex(float x, float y, float z)
    {
        *position++ = (short)(x * 64.0f);
        *position++ = (short)(y * 64.0f);
        *position++ = (short)(z * 64.0f);
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

    int count;
    glModel* model;
    void* resource;
    short* position;
    short* texcoord;
    u32* colour;
}; // size 0x18

#endif // GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H
