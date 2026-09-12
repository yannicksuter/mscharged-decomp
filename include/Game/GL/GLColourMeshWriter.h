#ifndef GAME_GL_GL_COLOUR_MESH_WRITER_H
#define GAME_GL_GL_COLOUR_MESH_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class GLColourMeshWriter
{
public:
    GLColourMeshWriter();
    ~GLColourMeshWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& c)
    {
        ColourPlat(*(const unsigned long*)&c);
    }

    void ColourPlat(unsigned long nColourPlat)
    {
        *colour++ = nColourPlat;
    }

    void Colour(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
    {
        nlColour colour;
        nlColourSet(colour, r, g, b, a);
        Colour(colour);
    }

    void Vertex(const nlVector3& value)
    {
        float x;
        float y;
        float z;

        z = value.z;
        y = value.y;
        x = value.x;
        Vertex(x, y, z);
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
}; // size 0x14

#endif // GAME_GL_GL_COLOUR_MESH_WRITER_H
