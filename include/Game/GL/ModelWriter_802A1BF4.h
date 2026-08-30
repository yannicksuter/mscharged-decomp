#ifndef GAME_GL_MODEL_WRITER_802A1BF4_H
#define GAME_GL_MODEL_WRITER_802A1BF4_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"

struct ModelWriter_802A1BF4
{
    int count;
    glModel* model;
    void* allocator;
    float* position;
    short* texcoord;
    u32* colour;

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

extern "C" void fn_802A1BF4(ModelWriter_802A1BF4* writer);
extern "C" void* fn_802A1C14(
    ModelWriter_802A1BF4* writer, int shouldDelete);
extern "C" bool fn_802A1C54(ModelWriter_802A1BF4* writer,
    int vertexCount, int primitive, void* allocator);
extern "C" bool fn_802A1E00(ModelWriter_802A1BF4* writer);

#endif // GAME_GL_MODEL_WRITER_802A1BF4_H
