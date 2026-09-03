#ifndef UNCLASSIFIED_TU_802A8A90_H
#define UNCLASSIFIED_TU_802A8A90_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"

#include "types.h"

class UnidentifiedMeshWriter_802A8A90
{
public:
    UnidentifiedMeshWriter_802A8A90();
    ~UnidentifiedMeshWriter_802A8A90();

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

#endif // UNCLASSIFIED_TU_802A8A90_H
