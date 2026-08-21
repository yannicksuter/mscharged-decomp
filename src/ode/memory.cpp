/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#include <ode/config.h>
#include <ode/memory.h>

extern "C" void* malloc(size_t size);
extern "C" void free(void* ptr);

static dAllocFunction* allocfn = 0;
static dReallocFunction* reallocfn = 0;
static dFreeFunction* freefn = 0;

/* MWCC emits this TU's function bodies in reverse source order. */

void dFree(void* ptr, size_t size)
{
    if (!ptr)
        return;
    if (freefn)
        freefn(ptr, size);
    else
        free(ptr);
}

void* dAlloc(size_t size)
{
    if (allocfn)
        return allocfn(size);
    else
        return malloc(size);
}

void dSetFreeHandler(dFreeFunction* fn)
{
    freefn = fn;
}

void dSetReallocHandler(dReallocFunction* fn)
{
    reallocfn = fn;
}

void dSetAllocHandler(dAllocFunction* fn)
{
    allocfn = fn;
}
