#ifndef GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H
#define GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H

#include "NL/gl/glModel.h"

class GLCompactColourMeshWriter
{
public:
    GLCompactColourMeshWriter();
    ~GLCompactColourMeshWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    int count;
    glModel* model;
    void* resource;
    short* position;
    short* texcoord;
    u32* colour;
}; // size 0x18

#endif // GAME_GL_GL_COMPACT_COLOUR_MESH_WRITER_H
