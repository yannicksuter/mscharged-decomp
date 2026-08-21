/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_OBSTACK_H
#define ODE_OBSTACK_H

#include "objects.h"

#define dOBSTACK_ARENA_SIZE 16384

struct dObStack : public dBase
{
    struct Arena
    {
        Arena* next;
        int used;
    };

    Arena* first;
    Arena* last;
    Arena* current_arena;
    int current_ofs;

    dObStack();
    ~dObStack();

    void* alloc(int num_bytes);
    void freeAll();
    void* rewind();
    void* next(int num_bytes);
};

#endif
