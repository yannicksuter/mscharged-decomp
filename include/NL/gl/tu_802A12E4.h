#ifndef NL_GL_TU_802A12E4_H
#define NL_GL_TU_802A12E4_H

#include "NL/gl/glModel.h"

class State_802A12E4
{
public:
    State_802A12E4();
    ~State_802A12E4();
    bool fn_802A1344(int vertexCount, int primitive, void* allocator);
    bool fn_802A14F0();

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* value_0C;
    /* 0x10 */ short* value_10;
    /* 0x14 */ u32* value_14;
};

#endif // NL_GL_TU_802A12E4_H
