#ifndef NL_GL_SHADOWED_TEXTURED_COLOUR_MODEL_WRITER_H
#define NL_GL_SHADOWED_TEXTURED_COLOUR_MODEL_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class glShadowedTexturedColourModelWriter
{
public:
    glShadowedTexturedColourModelWriter();
    ~glShadowedTexturedColourModelWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        *colour++ = *(const unsigned long*)&c;
    }

    void Texcoord(const nlVector2& uv)
    {
        short u = (short)(uv.x * 1024.0f);
        short v = (short)(uv.y * 1024.0f);
        *texcoord++ = u;
        *texcoord++ = v;
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
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* position;
    /* 0x10 */ short* texcoord;
    /* 0x14 */ u32* colour;
};

#endif // NL_GL_SHADOWED_TEXTURED_COLOUR_MODEL_WRITER_H
