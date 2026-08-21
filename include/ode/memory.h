/*************************************************************************
 * Open Dynamics Engine, Copyright (C) 2001-2004 Russell L. Smith.
 * Distributed under the BSD-style license in LICENSES/ODE-BSD.txt.
 *************************************************************************/

#ifndef ODE_MEMORY_H
#define ODE_MEMORY_H

#include <ode/config.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void* dAllocFunction(size_t size);
    typedef void* dReallocFunction(void* ptr, size_t oldsize, size_t newsize);
    typedef void dFreeFunction(void* ptr, size_t size);

    void dSetAllocHandler(dAllocFunction* fn);
    void dSetReallocHandler(dReallocFunction* fn);
    void dSetFreeHandler(dFreeFunction* fn);

    dAllocFunction* dGetAllocHandler();
    dReallocFunction* dGetReallocHandler();
    dFreeFunction* dGetFreeHandler();

    void* dAlloc(size_t size);
    void* dRealloc(void* ptr, size_t oldsize, size_t newsize);
    void dFree(void* ptr, size_t size);

#ifdef __cplusplus
}
#endif

#endif
