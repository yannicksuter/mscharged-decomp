#ifndef NL_GL_MULTI_TEXTURE_MODEL_WRITER_H
#define NL_GL_MULTI_TEXTURE_MODEL_WRITER_H

#include "NL/gl/glModel.h"

class glMultiTextureModelWriter
{
public:
    glMultiTextureModelWriter();
    ~glMultiTextureModelWriter();
    bool Begin(int vertexCount, int primitive, void* allocator);
    bool End();

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
