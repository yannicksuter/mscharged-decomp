#ifndef GAME_GL_MODEL_WRITER_802A1BF4_H
#define GAME_GL_MODEL_WRITER_802A1BF4_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"

struct ModelWriter_802A1BF4
{
    ModelWriter_802A1BF4();
    ~ModelWriter_802A1BF4();
    bool fn_802A1C54(int vertexCount, int primitive, void* allocator);
    bool fn_802A1E00();

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* allocator;
    /* 0x0C */ float* position;
    /* 0x10 */ short* texcoord;
    /* 0x14 */ u32* colour;

    void Colour(const nlColour& c)
    {
        *colour++ = *(const u32*)&c;
    }

    void Texcoord(short u, short v)
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
}; // size: 0x18

#endif // GAME_GL_MODEL_WRITER_802A1BF4_H
