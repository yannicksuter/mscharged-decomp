#ifndef NL_GL_TU_802A12E4_H
#define NL_GL_TU_802A12E4_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class State_802A12E4
{
public:
    State_802A12E4();
    ~State_802A12E4();
    bool fn_802A1344(int vertexCount, int primitive, void* allocator);
    bool fn_802A14F0();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        *value_14++ = *(const unsigned long*)&c;
    }

    void Texcoord(const nlVector2& uv)
    {
        short u = (short)(uv.x * 1024.0f);
        short v = (short)(uv.y * 1024.0f);
        *value_10++ = u;
        *value_10++ = v;
    }

    void Vertex(const nlVector3& pos)
    {
        float x;
        float y;
        float z;

        z = pos.z;
        y = pos.y;
        x = pos.x;
        Vertex(x, y, z);
    }

    void Vertex(float x, float y, float z)
    {
        *value_0C++ = x;
        *value_0C++ = y;
        *value_0C++ = z;
    }

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* value_0C;
    /* 0x10 */ short* value_10;
    /* 0x14 */ u32* value_14;
};

#endif // NL_GL_TU_802A12E4_H
