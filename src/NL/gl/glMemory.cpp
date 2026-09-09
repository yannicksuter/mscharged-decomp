#include "NL/gl/glMemory.h"

#include "Game/GL/GLInventory.h"
#include "NL/nlDLRing.h"
#include "NL/nlMemory.h"

static GLResourcePool* sCurrentResourcePool;
static GLResourcePool* sResourcePoolList;

void glResourceAllocationFailed()
{
}

void glInitResourcePools()
{
    sCurrentResourcePool = 0;
    sResourcePoolList = 0;
}

GLResourcePool::GLResourcePool()
{
    m_level = 0;
    m_inventory = new (8, false) GLInventory;
    m_inventory->Create();
    m_prev = 0;
    m_next = 0;
}

GLResourcePool::~GLResourcePool()
{
    if (m_inventory != 0)
    {
        delete m_inventory;
        m_inventory = 0;
    }

    if (this == sCurrentResourcePool)
    {
        sCurrentResourcePool = 0;
    }
}

GLResourcePool* glCreateResourcePool(
    const GLMemoryRequirement* requirements, int count, const char* name)
{
    GLResourcePool* resource
        = glplatCreateResourcePool(
            requirements,
            count, name);
    nlDLRingAddEnd(&sResourcePoolList, resource);
    return resource;
}

void glDestroyResourcePool(GLResourcePool* resource)
{
    nlDLRingRemove(&sResourcePoolList, resource);
    delete resource;
}

void glSetCurrentResourcePool(GLResourcePool* resource)
{
    sCurrentResourcePool = resource;
}

GLResourcePool* glGetCurrentResourcePool()
{
    return sCurrentResourcePool;
}

GLResourcePool* glGetResourcePools()
{
    return sResourcePoolList;
}

void* glResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource)
{
    return glplatResourceAlloc(size, memType, resource);
}

void* glFrameAlloc(unsigned long size, eGLMemory memType)
{
    return glplatFrameAlloc(size, memType);
}
