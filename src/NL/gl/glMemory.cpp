#include "NL/gl/glMemory.h"

#include "Game/GL/GLInventory.h"
#include "NL/nlDLRing.h"
#include "NL/nlMemory.h"

extern "C" ResourceInterface_802CC094* fn_80376664(
    const void* configuration, int count, const char* name);

static ResourceInterface_802CC094* lbl_806E1ED8;
static ResourceInterface_802CC094* lbl_806E1EDC;

extern "C" void fn_802CBEC4()
{
}

extern "C" void fn_802CBEC8()
{
    lbl_806E1ED8 = 0;
    lbl_806E1EDC = 0;
}

ResourceInterface_802CC094::ResourceInterface_802CC094()
{
    m_level = 0;
    m_inventory = new (8, false) GLInventory;
    m_inventory->Create();
    m_prev = 0;
    m_next = 0;
}

ResourceInterface_802CC094::~ResourceInterface_802CC094()
{
    if (m_inventory != 0)
    {
        delete m_inventory;
        m_inventory = 0;
    }

    if (this == lbl_806E1ED8)
    {
        lbl_806E1ED8 = 0;
    }
}

extern "C" ResourceInterface_802CC094* fn_802CBFD8(
    const void* configuration, int count, const char* name)
{
    ResourceInterface_802CC094* resource
        = fn_80376664(configuration, count, name);
    nlDLRingAddEnd(&lbl_806E1EDC, resource);
    return resource;
}

extern "C" void fn_802CC02C(ResourceInterface_802CC094* resource)
{
    nlDLRingRemove(&lbl_806E1EDC, resource);
    delete resource;
}

extern "C" void fn_802CC08C(ResourceInterface_802CC094* resource)
{
    lbl_806E1ED8 = resource;
}

extern "C" ResourceInterface_802CC094* fn_802CC094()
{
    return lbl_806E1ED8;
}

extern "C" ResourceInterface_802CC094* fn_802CC09C()
{
    return lbl_806E1EDC;
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
