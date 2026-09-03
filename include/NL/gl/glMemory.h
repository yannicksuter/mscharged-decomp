#ifndef NL_GL_GLMEMORY_H
#define NL_GL_GLMEMORY_H

#include "NL/glx/glxMemory.h"

class GLInventory;

class ResourceInterface_802CC094
{
public:
    ResourceInterface_802CC094();

    virtual void* Allocate(unsigned long size, eGLMemory memType) = 0;
    virtual unsigned long MarkResource() = 0;
    virtual void ReleaseResource(unsigned long marker) = 0;
    virtual unsigned long GetFreeMemory() = 0;
    virtual unsigned long fn_18() = 0;
    virtual unsigned long fn_1C();
    virtual unsigned long fn_20();
    virtual unsigned long fn_24();
    virtual unsigned long fn_28();
    virtual bool fn_2C(unsigned long, const char**, unsigned long*,
        unsigned long*, unsigned long*, const char**);
    virtual ~ResourceInterface_802CC094();

    ResourceInterface_802CC094* m_next;
    ResourceInterface_802CC094* m_prev;
    GLInventory* m_inventory;
    int m_level;
}; // size: 0x14

extern "C"
{
void fn_802CBEC4();
void fn_802CBEC8();
ResourceInterface_802CC094* fn_802CBFD8(
    const void* configuration, int count, const char* name);
void fn_802CC02C(ResourceInterface_802CC094* resource);
void fn_802CC08C(ResourceInterface_802CC094* resource);
ResourceInterface_802CC094* fn_802CC094();
ResourceInterface_802CC094* fn_802CC09C();
}

void* glResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource);
void* glFrameAlloc(unsigned long size, eGLMemory memType);

#endif // NL_GL_GLMEMORY_H
