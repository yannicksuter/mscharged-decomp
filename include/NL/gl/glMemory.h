#ifndef NL_GL_GLMEMORY_H
#define NL_GL_GLMEMORY_H

#include "NL/glx/glxMemory.h"

class GLInventory;
struct glModelPacket;

class GLResourcePool
{
public:
    GLResourcePool();

    virtual void* Allocate(unsigned long size, eGLMemory memType) = 0;
    virtual unsigned long MarkResource() = 0;
    virtual void ReleaseResource(unsigned long marker) = 0;
    virtual unsigned long GetFreeMemory() = 0;
    virtual unsigned long GetTotalMemory() = 0;
    virtual unsigned long GetPeakMemoryUsage();
    virtual unsigned long fn_20();
    virtual unsigned long fn_24();
    virtual unsigned long fn_28();
    virtual bool GetPoolMemoryInfo(unsigned long, const char**, unsigned long*,
        unsigned long*, unsigned long*, const char**);
    virtual ~GLResourcePool();

    GLResourcePool* m_next;
    GLResourcePool* m_prev;
    GLInventory* m_inventory;
    int m_level;
}; // size: 0x14

void glResourceAllocationFailed();
void glInitResourcePools();
GLResourcePool* glCreateResourcePool(
    const GLMemoryRequirement* requirements, int count, const char* name);
void glDestroyResourcePool(GLResourcePool* resource);
void glSetCurrentResourcePool(GLResourcePool* resource);
GLResourcePool* glGetCurrentResourcePool();
GLResourcePool* glGetResourcePools();

// Platform hook applied after a packet and its material data have been cloned.
void glplatOnPacketCloned(glModelPacket* packet, void* allocator);

void* glResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource);
void* glFrameAlloc(unsigned long size, eGLMemory memType);

#endif // NL_GL_GLMEMORY_H
