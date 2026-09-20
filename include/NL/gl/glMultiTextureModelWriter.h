#ifndef NL_GL_MULTI_TEXTURE_MODEL_WRITER_H
#define NL_GL_MULTI_TEXTURE_MODEL_WRITER_H

#include "NL/gl/glModel.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

class glMultiTextureModelWriter
{
public:
    glMultiTextureModelWriter();
    ~glMultiTextureModelWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

    glModel* GetModel() const
    {
        return model;
    }

    void Colour(const nlColour& colour)
    {
        ColourPlat(*(const u32*)&colour);
    }

    void ColourPlat(u32 colour)
    {
        *colours++ = colour;
    }

    void Texcoord0(const nlVector2& value)
    {
        Texcoord0(value.x, value.y);
    }

    void Texcoord0(float u, float v)
    {
        *texcoords0++ = u;
        *texcoords0++ = v;
    }

    void Texcoord1(const nlVector2& value)
    {
        Texcoord1(value.x, value.y);
    }

    void Texcoord1(float u, float v)
    {
        *texcoords1++ = u;
        *texcoords1++ = v;
    }

    void Texcoord2(const nlVector2& value)
    {
        Texcoord2(value.x, value.y);
    }

    void Texcoord2(float u, float v)
    {
        *texcoords2++ = u;
        *texcoords2++ = v;
    }

    void Vertex(const nlVector3& value)
    {
        Vertex(value.x, value.y, value.z);
    }

    void Vertex(float x, float y, float z)
    {
        *positions++ = x;
        *positions++ = y;
        *positions++ = z;
    }

    void Texture(int index, u32 texture)
    {
        glTextureBinding* binding
            = (glTextureBinding*)model->packets->materialParameters + index;
        binding->texture = texture;
        binding->textureIndex = 0xFFFF;
        binding->SetWrapS(true);
        binding->SetWrapT(true);
        binding->unknown07 = 0;
    }

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* positions;
    /* 0x10 */ float* texcoords0;
    /* 0x14 */ float* texcoords1;
    /* 0x18 */ float* texcoords2;
    /* 0x1C */ u32* colours;
};

#endif // NL_GL_MULTI_TEXTURE_MODEL_WRITER_H
