#ifndef GAME_GL_GLWARBLEMESHWRITER_H
#define GAME_GL_GLWARBLEMESHWRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"

struct GLWarbleMeshWriter
{
    GLWarbleMeshWriter();
    ~GLWarbleMeshWriter();
    bool Begin(int numVerts, int prim, void* allocator);
    bool End();

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

    void Position(float x, float y, float z)
    {
        *position++ = x;
        *position++ = y;
        *position++ = z;
    }
}; // size: 0x18

#endif // GAME_GL_GLWARBLEMESHWRITER_H
