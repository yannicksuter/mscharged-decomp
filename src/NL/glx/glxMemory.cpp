#include "NL/glx/glxMemory.h"

#include "Game/GL/GLInventory.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glMemory.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern "C"
{
void GXInvalidateVtxCache();
void GXInvalidateTexAll();
void OSReport(const char*, ...);
void fn_8004F594(int, const char*, ...);
void fn_802CBEC4();
void fn_802C828C(const char*, void*);
}

class UnidentifiedResourceAllocator_80376234;

struct UnidentifiedResourceMarker_803764E0
{
    unsigned long m_00[2];
    int m_08;
    UnidentifiedResourceAllocator_80376234* m_0C;
};

struct UnidentifiedMemoryRequirement_80376664
{
    eGLMemory m_00;
    unsigned long m_04;
};

class UnidentifiedResourceAllocator_80376234
    : public ResourceInterface_802CC094
{
public:
    UnidentifiedResourceAllocator_80376234(
        unsigned long sizes[2], const char* name);

    virtual void* Allocate(unsigned long size, eGLMemory memType);
    virtual unsigned long MarkResource();
    virtual void ReleaseResource(unsigned long marker);
    virtual unsigned long GetFreeMemory();
    virtual unsigned long fn_18();
    virtual unsigned long fn_1C();
    virtual bool fn_2C(unsigned long, const char**, unsigned long*,
        unsigned long*, unsigned long*, const char**);
    virtual ~UnidentifiedResourceAllocator_80376234();

    char m_14[32];
    unsigned long m_34[2];
    unsigned long m_3C[2];
    unsigned long m_44[2];
    unsigned long m_4C[2];
}; // size: 0x54

static char lbl_80535F08[] = "out of frame memory (%s)\n";
static char lbl_80535F24[] =
    "memory used: %uKB frame MEM1, %uKB frame MEM2\n";
static char lbl_80535F54[] =
    "       free: %uKB frame MEM1, %uKB frame MEM2\n";
static char lbl_80535F84[] =
    "Out of RL resource memory (%s) in pool %s\n";
static char lbl_80535FB0[] = "OutOfMem";

static char lbl_806DFB10[] = "MEM2";
static char lbl_806DFB18[] = "MEM1";

static unsigned char glx_MemoryDump;
static unsigned long FrameMemSizes[2];
static int i_frame;
static bool lbl_806E2494;
static void* lbl_806E2498[2];

static unsigned long p_frame[2][2];
static unsigned long n_frame[2][2];

static inline int RealOrVirtual(eGLMemory memType)
{
    switch (memType)
    {
    case GLM_Header:
    case GLM_Matrix:
    case GLM_IndexData:
    case GLM_Target:
        return 0;
    case GLM_VertexData:
    case GLM_TextureData:
        return 1;
    default:
        nlBreak();
        return -1;
    }
}

bool glxInitMemory(
    unsigned long frameMemSize1, unsigned long frameMemSize2)
{
    MemoryAllocator* allocator = CurrentAllocator;
    CurrentAllocator = &StandardAllocator;
    unsigned long pMem = (unsigned long)nlMalloc(
        frameMemSize1 * 2, 32, false);
    if (pMem == 0)
    {
        return false;
    }

    p_frame[0][0] = pMem;
    p_frame[1][0] = pMem + frameMemSize1;

    CurrentAllocator = &VirtualAllocator;
    pMem = (unsigned long)nlMalloc(frameMemSize2 * 2, 32, false);
    if (pMem == 0)
    {
        return false;
    }

    p_frame[0][1] = pMem;
    p_frame[1][1] = pMem + frameMemSize2;
    CurrentAllocator = allocator;

    i_frame = 0;
    n_frame[1][0] = 0;
    n_frame[0][0] = 0;
    n_frame[1][1] = 0;
    n_frame[0][1] = 0;
    FrameMemSizes[0] = frameMemSize1;
    FrameMemSizes[1] = frameMemSize2;
    lbl_806E2494 = true;
    return true;
}

void glplatFrameAllocNextFrame()
{
    if (glx_MemoryDump)
    {
        fn_8004F594(2, lbl_80535F24,
            n_frame[i_frame][0] >> 10, n_frame[i_frame][1] >> 10);
        fn_8004F594(2, lbl_80535F54,
            (FrameMemSizes[0] - n_frame[i_frame][0]) >> 10,
            (FrameMemSizes[1] - n_frame[i_frame][1]) >> 10);
        glx_MemoryDump = false;
    }

    int newFrame = i_frame ^ 1;
    i_frame = newFrame;
    n_frame[newFrame][0] = 0;
    n_frame[newFrame][1] = 0;
    GXInvalidateVtxCache();
    GXInvalidateTexAll();
}

UnidentifiedResourceAllocator_80376234::
    UnidentifiedResourceAllocator_80376234(
        unsigned long sizes[2], const char* name)
{
    nlStrNCpy(m_14, name, 32);

    m_inventory->fn_802D19C4(lbl_806E2498);

    MemoryAllocator* allocator = CurrentAllocator;
    CurrentAllocator = &StandardAllocator;
    m_34[0] = (unsigned long)nlMalloc(sizes[0], 32, false);
    CurrentAllocator = &VirtualAllocator;
    m_34[1] = (unsigned long)nlMalloc(sizes[1], 32, false);
    CurrentAllocator = allocator;

    m_3C[0] = sizes[0];
    m_44[0] = 0;
    m_4C[0] = 0;
    m_3C[1] = sizes[1];
    m_44[1] = 0;
    m_4C[1] = 0;
}

UnidentifiedResourceAllocator_80376234::
    ~UnidentifiedResourceAllocator_80376234()
{
    for (int i = 0; i < 2; ++i)
    {
        operator delete((void*)m_34[i]);
        m_44[i] = 0;
        m_3C[i] = 0;
    }
}

void* UnidentifiedResourceAllocator_80376234::Allocate(
    unsigned long allocationSize, eGLMemory memType)
{
    int pool = RealOrVirtual(memType);
    unsigned long aligned = (m_34[pool] + m_44[pool] + 31) & ~31u;
    m_44[pool] = allocationSize + (aligned - m_34[pool]);
    m_4C[pool] = m_44[pool] > m_4C[pool]
        ? m_44[pool]
        : m_4C[pool];

    if (m_44[pool] > m_3C[pool])
    {
        extern const char* lbl_8052BFB8[];
        OSReport(lbl_80535F84, lbl_8052BFB8[memType], m_14);
        fn_802CBEC4();
        fn_802C828C(lbl_80535FB0, this);
        nlBreak();
    }
    return (void*)aligned;
}

unsigned long UnidentifiedResourceAllocator_80376234::MarkResource()
{
    unsigned long value0 = m_44[0];
    unsigned long value1 = m_44[1];
    UnidentifiedResourceMarker_803764E0* marker
        = (UnidentifiedResourceMarker_803764E0*)glResourceAlloc(
            sizeof(UnidentifiedResourceMarker_803764E0), GLM_Header, this);
    marker->m_00[0] = value0;
    marker->m_00[1] = value1;
    marker->m_08 = m_level;
    marker->m_0C = this;
    m_inventory->ResourceMark();
    m_level++;
    return (unsigned long)marker;
}

void UnidentifiedResourceAllocator_80376234::ReleaseResource(
    unsigned long value)
{
    UnidentifiedResourceMarker_803764E0* marker
        = (UnidentifiedResourceMarker_803764E0*)value;
    int level = marker->m_08;
    m_inventory->ResourceRelease(level);
    m_44[0] = marker->m_00[0];
    m_44[1] = marker->m_00[1];
    m_level = level;
}

unsigned long UnidentifiedResourceAllocator_80376234::GetFreeMemory()
{
    unsigned long value0 = m_3C[0] - m_44[0];
    unsigned long value1 = m_3C[1] - m_44[1];
    return value0 + value1;
}

unsigned long UnidentifiedResourceAllocator_80376234::fn_18()
{
    return m_3C[0] + m_3C[1];
}

unsigned long UnidentifiedResourceAllocator_80376234::fn_1C()
{
    return m_4C[0] + m_4C[1];
}

bool UnidentifiedResourceAllocator_80376234::fn_2C(
    unsigned long arg0, const char** arg1, unsigned long* arg2,
    unsigned long* arg3, unsigned long* arg4, const char** arg5)
{
    if (arg0 > 1)
    {
        return false;
    }
    *arg1 = arg0 != 0 ? lbl_806DFB10 : lbl_806DFB18;
    *arg2 = m_3C[arg0];
    *arg3 = m_3C[arg0] - m_44[arg0];
    *arg4 = m_4C[arg0];
    if (arg5 != 0)
    {
        *arg5 = "";
    }
    return true;
}

extern "C" ResourceInterface_802CC094* fn_80376664(
    const UnidentifiedMemoryRequirement_80376664* requirements,
    unsigned long count, const char* name)
{
    unsigned long sizes[2] = { 0, 0 };
    for (unsigned long i = 0; i < count; ++i)
    {
        int pool = RealOrVirtual(requirements[i].m_00);
        sizes[pool] += requirements[i].m_04;
    }

    return new (8, false)
        UnidentifiedResourceAllocator_80376234(sizes, name);
}

void* glplatResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource)
{
    return ((ResourceInterface_802CC094*)resource)->Allocate(
        size, memType);
}

void* glplatFrameAlloc(unsigned long size, eGLMemory memType)
{
    unsigned long isLow = RealOrVirtual(memType);
    unsigned long newTop
        = (p_frame[i_frame][isLow] + n_frame[i_frame][isLow] + 31)
        & ~31u;
    unsigned long newSize = size + (newTop - p_frame[i_frame][isLow]);
    if (newSize > FrameMemSizes[isLow])
    {
        extern const char* lbl_8052BFB8[];
        OSReport(lbl_80535F08, lbl_8052BFB8[memType]);
        nlBreak();
        return 0;
    }
    n_frame[i_frame][isLow] = newSize;
    return (void*)newTop;
}

unsigned long glx_GetFreeMemory()
{
    return fn_802CC094()->GetFreeMemory();
}
