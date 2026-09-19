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
        short u = (short)(uv.x * 1024.0f);
        short v = (short)(uv.y * 1024.0f);
        Texcoord(u, v);
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

    int count;
    glModel* model;
    void* resource;
    short* position;
    short* texcoord;
    u32* colour;
}; // size 0x18

#endif // GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H
