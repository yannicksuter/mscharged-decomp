#ifndef GAME_GL_GL_FLOAT_TEXTURED_COLOUR_MESH_WRITER_H
#define GAME_GL_GL_FLOAT_TEXTURED_COLOUR_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLFloatTexturedColourMeshWriter
{
public:
    GLFloatTexturedColourMeshWriter();
    ~GLFloatTexturedColourMeshWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        *colour++ = *(const u32*)&c;
    }

    void Texcoord(const nlVector2& uv)
    {
        Texcoord(uv.x, uv.y);
    }

    void Texcoord(float u, float v)
    {
        *texcoord++ = u;
        *texcoord++ = v;
    }

    void TexcoordZero()
    {
        *texcoord++ = 0.0f;
        *texcoord++ = 0.0f;
    }

    void Vertex(float x, float y, float z)
    {
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }

    int count;
    glModel* model;
    void* resource;
    float* position;
    float* texcoord;
    u32* colour;
}; // size 0x18

#endif // GAME_GL_GL_FLOAT_TEXTURED_COLOUR_MESH_WRITER_H
