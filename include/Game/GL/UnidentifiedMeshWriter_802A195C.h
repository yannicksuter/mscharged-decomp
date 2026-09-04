#ifndef GAME_GL_UNIDENTIFIED_MESH_WRITER_802A195C_H
#define GAME_GL_UNIDENTIFIED_MESH_WRITER_802A195C_H

#include "NL/gl/glModel.h"
#include "NL/nlMath.h"

// Mesh writer variant at 0x802A195C with float texture coordinates. The
// stripped DOL does not retain the original class name.
class UnidentifiedMeshWriter_802A195C
{
public:
    UnidentifiedMeshWriter_802A195C();
    ~UnidentifiedMeshWriter_802A195C();
    bool Begin(int count, int primitive, void* resource);
    bool End();

    void Texcoord(const nlVector2& value)
    {
        float x;
        float y;
        y = value.y;
        x = value.x;
        *texcoord++ = x;
        *texcoord++ = y;
    }

    void Vertex(float x, float y, float z)
    {
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* position;
    /* 0x10 */ float* texcoord;
}; // size: 0x14

#endif // GAME_GL_UNIDENTIFIED_MESH_WRITER_802A195C_H
