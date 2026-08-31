#ifndef GAME_GL_GL_MESH_WRITER_H
#define GAME_GL_GL_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLMeshWriter
{
public:
    GLMeshWriter();
    ~GLMeshWriter();

    bool Begin(int numVerts, int prim, void* pResource);
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
        short u = (short)(uv.x * 1024.0f);
        short v = (short)(uv.y * 1024.0f);
        *texcoord++ = u;
        *texcoord++ = v;
    }

    void Texcoord(short u, short v)
    {
        *texcoord++ = u;
        *texcoord++ = v;
    }

    void TexcoordZero()
    {
        *texcoord++ = 0;
        *texcoord++ = 0;
    }

    void Vertex(const nlVector3& pos)
    {
        float x;
        float y;
        float z;
        z = pos.z;
        y = pos.y;
        x = pos.x;
        *position++ = x;
        *position++ = y;
        *position++ = z;
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
    short* texcoord;
    u32* colour;
}; // size 0x18

#endif // GAME_GL_GL_MESH_WRITER_H
