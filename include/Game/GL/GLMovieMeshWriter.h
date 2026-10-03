#ifndef GAME_GL_GL_MOVIE_MESH_WRITER_H
#define GAME_GL_GL_MOVIE_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlMath.h"

// Builds models for the movie material program (GXMovieMaterialProgram):
// a position stream and a float texture-coordinate stream, no colours.
class GLMovieMeshWriter
{
public:
    GLMovieMeshWriter();
    ~GLMovieMeshWriter();
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

#endif // GAME_GL_GL_MOVIE_MESH_WRITER_H
