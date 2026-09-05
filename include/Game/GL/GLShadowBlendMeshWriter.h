#ifndef GAME_GL_GL_SHADOW_BLEND_MESH_WRITER_H
#define GAME_GL_GL_SHADOW_BLEND_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"

#include "types.h"

class GLShadowBlendMeshWriter
{
public:
    GLShadowBlendMeshWriter();
    ~GLShadowBlendMeshWriter();

    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& value)
    {
        *colour++ = *(const u32*)&value;
    }

    void Texcoord(float u, float v)
    {
        *texcoord++ = u;
        *texcoord++ = v;
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
    u32* colour;
    float* texcoord;
}; // size: 0x18

#endif // GAME_GL_GL_SHADOW_BLEND_MESH_WRITER_H
