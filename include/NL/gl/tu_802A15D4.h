#ifndef NL_GL_TU_802A15D4_H
#define NL_GL_TU_802A15D4_H

#include "NL/gl/glModel.h"

class State_802A15D4
{
public:
    State_802A15D4();
    ~State_802A15D4();
    bool fn_802A163C(int vertexCount, int primitive, void* allocator);
    bool fn_802A1878();

    /* 0x00 */ int count;
    /* 0x04 */ glModel* model;
    /* 0x08 */ void* resource;
    /* 0x0C */ float* value_0C;
    /* 0x10 */ float* value_10;
    /* 0x14 */ float* value_14;
    /* 0x18 */ float* value_18;
    /* 0x1C */ u32* value_1C;
};

#endif // NL_GL_TU_802A15D4_H
