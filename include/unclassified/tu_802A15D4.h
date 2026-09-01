#ifndef UNCLASSIFIED_TU_802A15D4_H
#define UNCLASSIFIED_TU_802A15D4_H

#include "NL/gl/glModel.h"
#include "types.h"

struct State_802A15D4
{
    int count;
    glModel* model;
    void* resource;
    float* value_0C;
    float* value_10;
    float* value_14;
    float* value_18;
    u32* value_1C;
};

extern "C" void fn_802A15D4(State_802A15D4* writer);
extern "C" void* fn_802A15FC(
    State_802A15D4* writer, int shouldDelete);
extern "C" bool fn_802A163C(State_802A15D4* writer,
    int vertexCount, int primitive, void* allocator);
extern "C" bool fn_802A1878(State_802A15D4* writer);

#endif // UNCLASSIFIED_TU_802A15D4_H
