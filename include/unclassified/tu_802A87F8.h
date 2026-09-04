#ifndef UNCLASSIFIED_TU_802A87F8_H
#define UNCLASSIFIED_TU_802A87F8_H

#include "NL/gl/glModel.h"
#include "types.h"

struct State_802A87F8
{
    int count;
    glModel* model;
    void* resource;
    float* position;
    u32* colour;
};

extern "C"
{
    void fn_802A87F8(State_802A87F8* writer);
    void* fn_802A8814(State_802A87F8* writer, int shouldDelete);
    bool fn_802A8854(State_802A87F8* writer,
        int vertexCount, int primitive, void* allocator);
    bool fn_802A89AC(State_802A87F8* writer);
}

#endif // UNCLASSIFIED_TU_802A87F8_H
