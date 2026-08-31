#ifndef GAME_GL_MESH_WRITER_H
#define GAME_GL_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlMath.h"

class MeshWriter
{
public:
    MeshWriter();
    ~MeshWriter();
    bool Begin(int count, int primitive, void* resource);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Texcoord(const nlVector2& value)
    {
        *texcoord++ = (short)(value.x * 4096.0f);
        *texcoord++ = (short)(value.y * 4096.0f);
    }

    void Vertex(const nlVector3& value)
    {
        float x;
        float y;
        float z;
        z = value.z;
        y = value.y;
        x = value.x;
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }

    int count;
    glModel* model;
    void* resource;
    float* position;
    short* texcoord;
};

#endif // GAME_GL_MESH_WRITER_H
