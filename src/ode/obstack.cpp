/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This file is distributed under the BSD-style license in               *
 * LICENSES/ODE-BSD.txt.                                                 *
 *                                                                       *
 *************************************************************************/

#include <ode/common.h>
#include <ode/error.h>
#include <ode/memory.h>
#include "obstack.h"

#define ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(arena, ofs) \
    ofs = (size_t)(dEFFICIENT_SIZE(((intP)(arena)) + ofs) - ((intP)(arena)));

#define MAX_ALLOC_SIZE \
    ((size_t)(dOBSTACK_ARENA_SIZE - sizeof(Arena) - EFFICIENT_ALIGNMENT + 1))

void* dObStack::next(int num_bytes)
{
    if (!current_arena)
        return 0;
    current_ofs += num_bytes;
    ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs);
    if (current_ofs >= current_arena->used)
    {
        current_arena = current_arena->next;
        if (!current_arena)
            return 0;
        current_ofs = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs);
    }
    return ((char*)current_arena) + current_ofs;
}

void* dObStack::rewind()
{
    current_arena = first;
    current_ofs = sizeof(Arena);
    if (current_arena)
    {
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(current_arena, current_ofs)
        return ((char*)current_arena) + current_ofs;
    }
    else
        return 0;
}

void dObStack::freeAll()
{
    last = first;
    if (first)
    {
        first->used = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(first, first->used);
    }
}

void* dObStack::alloc(int num_bytes)
{
    if ((size_t)num_bytes > MAX_ALLOC_SIZE)
        dDebug(0, "num_bytes too large");

    if (!first)
    {
        first = last = (Arena*)dAlloc(dOBSTACK_ARENA_SIZE);
        first->next = 0;
        first->used = sizeof(Arena);
        ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(first, first->used);
    }
    else
    {
        if ((last->used + num_bytes) > dOBSTACK_ARENA_SIZE)
        {
            if (!last->next)
            {
                last->next = (Arena*)dAlloc(dOBSTACK_ARENA_SIZE);
                last->next->next = 0;
            }
            last = last->next;
            last->used = sizeof(Arena);
            ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(last, last->used);
        }
    }

    char* c = ((char*)last) + last->used;
    last->used += num_bytes;
    ROUND_UP_OFFSET_TO_EFFICIENT_SIZE(last, last->used);
    return c;
}

dObStack::~dObStack()
{
    Arena* a;
    Arena* nexta;
    a = first;
    while (a)
    {
        nexta = a->next;
        dFree(a, dOBSTACK_ARENA_SIZE);
        a = nexta;
    }
}

dObStack::dObStack()
{
    first = 0;
    last = 0;
    current_arena = 0;
    current_ofs = 0;
}
