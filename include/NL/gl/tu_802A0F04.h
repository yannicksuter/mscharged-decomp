#ifndef NL_GL_TU_802A0F04_H
#define NL_GL_TU_802A0F04_H

#include "NL/gl/glModel.h"

class State_802A0F04
{
public:
    State_802A0F04();
    ~State_802A0F04();
    bool fn_802A0F70(int vertexCount, int primitive, void* allocator);
    bool fn_802A1200();

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* value_0C;
    /* 0x10 */ float* value_10;
    /* 0x14 */ float* value_14;
    /* 0x18 */ float* value_18;
    /* 0x1C */ float* value_1C;
    /* 0x20 */ u32* value_20;
};

#endif // NL_GL_TU_802A0F04_H
