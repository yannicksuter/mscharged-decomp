#include <revolution/gx.h>
#include "Game/Sys/debug.h"
#include <revolution/os.h>

#include "NL/glx/glxMemory.h"

#include "Game/GL/GLInventory.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/nlDebug.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

class GLXResourcePool;

struct GLXResourceMarker
{
    unsigned long mUsedMemory[2];
    int mLevel;
    GLXResourcePool* mResource;
};

class GLXResourcePool
    : public GLResourcePool
{
public:
    GLXResourcePool(
        unsigned long sizes[2], const char* name);

    virtual void* Allocate(unsigned long size, eGLMemory memType);
    virtual unsigned long MarkResource();
    virtual void ReleaseResource(unsigned long marker);
    virtual unsigned long GetFreeMemory();
    virtual unsigned long GetTotalMemory();
    virtual unsigned long GetPeakMemoryUsage();
    virtual bool GetPoolMemoryInfo(unsigned long, const char**, unsigned long*,
        unsigned long*, unsigned long*, const char**);
    virtual ~GLXResourcePool();

    char mName[32];
    unsigned long mPoolMemory[2];
    unsigned long mPoolSizes[2];
    unsigned long mUsedMemory[2];
    unsigned long mPeakMemory[2];
}; // size: 0x54

static char sOutOfFrameMemoryFormat[] = "out of frame memory (%s)\n";
static char sFrameMemoryUsedFormat[] =
    "memory used: %uKB frame MEM1, %uKB frame MEM2\n";
static char sFrameMemoryFreeFormat[] =
    "       free: %uKB frame MEM1, %uKB frame MEM2\n";
static char sOutOfResourceMemoryFormat[] =
    "Out of RL resource memory (%s) in pool %s\n";
static char sOutOfMemoryName[] = "OutOfMem";

static char sMEM2Name[] = "MEM2";
static char sMEM1Name[] = "MEM1";

static unsigned char glx_MemoryDump;
static unsigned long FrameMemSizes[2];
static int i_frame;
static bool sFrameMemoryInitialized;
static GLInventory::ModelReleaseCallback sModelReleaseCallback;

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
    sFrameMemoryInitialized = true;
    return true;
}

void glplatFrameAllocNextFrame()
{
    if (glx_MemoryDump)
    {
        tDebugPrintManager::Print(DC_GLPLAT, sFrameMemoryUsedFormat,
            n_frame[i_frame][0] >> 10, n_frame[i_frame][1] >> 10);
        tDebugPrintManager::Print(DC_GLPLAT, sFrameMemoryFreeFormat,
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

GLXResourcePool::
    GLXResourcePool(
        unsigned long sizes[2], const char* name)
{
    nlStrNCpy(mName, name, 32);

    m_inventory->SetModelReleaseCallback(sModelReleaseCallback);

    MemoryAllocator* allocator = CurrentAllocator;
    CurrentAllocator = &StandardAllocator;
    mPoolMemory[0] = (unsigned long)nlMalloc(sizes[0], 32, false);
    CurrentAllocator = &VirtualAllocator;
    mPoolMemory[1] = (unsigned long)nlMalloc(sizes[1], 32, false);
    CurrentAllocator = allocator;

    mPoolSizes[0] = sizes[0];
    mUsedMemory[0] = 0;
    mPeakMemory[0] = 0;
    mPoolSizes[1] = sizes[1];
    mUsedMemory[1] = 0;
    mPeakMemory[1] = 0;
}

GLXResourcePool::
    ~GLXResourcePool()
{
    for (int i = 0; i < 2; ++i)
    {
        operator delete((void*)mPoolMemory[i]);
        mUsedMemory[i] = 0;
        mPoolSizes[i] = 0;
    }
}

void* GLXResourcePool::Allocate(
    unsigned long allocationSize, eGLMemory memType)
{
    int pool = RealOrVirtual(memType);
    unsigned long aligned = (mPoolMemory[pool] + mUsedMemory[pool] + 31) & ~31u;
    mUsedMemory[pool] = allocationSize + (aligned - mPoolMemory[pool]);
    mPeakMemory[pool] = mUsedMemory[pool] > mPeakMemory[pool]
        ? mUsedMemory[pool]
        : mPeakMemory[pool];

    if (mUsedMemory[pool] > mPoolSizes[pool])
    {
        OSReport(sOutOfResourceMemoryFormat, szMemoryNames[memType], mName);
        glResourceAllocationFailed();
        glDumpResources(sOutOfMemoryName, this);
        nlBreak();
    }
    return (void*)aligned;
}

unsigned long GLXResourcePool::MarkResource()
{
    unsigned long value0 = mUsedMemory[0];
    unsigned long value1 = mUsedMemory[1];
    GLXResourceMarker* marker
        = (GLXResourceMarker*)glResourceAlloc(
            sizeof(GLXResourceMarker), GLM_Header, this);
    marker->mUsedMemory[0] = value0;
    marker->mUsedMemory[1] = value1;
    marker->mLevel = m_level;
    marker->mResource = this;
    m_inventory->ResourceMark();
    m_level++;
    return (unsigned long)marker;
}

void GLXResourcePool::ReleaseResource(
    unsigned long value)
{
    GLXResourceMarker* marker
        = (GLXResourceMarker*)value;
    int level = marker->mLevel;
    m_inventory->ResourceRelease(level);
    mUsedMemory[0] = marker->mUsedMemory[0];
    mUsedMemory[1] = marker->mUsedMemory[1];
    m_level = level;
}

unsigned long GLXResourcePool::GetFreeMemory()
{
    unsigned long value0 = mPoolSizes[0] - mUsedMemory[0];
    unsigned long value1 = mPoolSizes[1] - mUsedMemory[1];
    return value0 + value1;
}

unsigned long GLXResourcePool::GetTotalMemory()
{
    return mPoolSizes[0] + mPoolSizes[1];
}

unsigned long GLXResourcePool::GetPeakMemoryUsage()
{
    return mPeakMemory[0] + mPeakMemory[1];
}

bool GLXResourcePool::GetPoolMemoryInfo(
    unsigned long arg0, const char** arg1, unsigned long* arg2,
    unsigned long* arg3, unsigned long* arg4, const char** arg5)
{
    if (arg0 > 1)
    {
        return false;
    }
    *arg1 = arg0 != 0 ? sMEM2Name : sMEM1Name;
    *arg2 = mPoolSizes[arg0];
    *arg3 = mPoolSizes[arg0] - mUsedMemory[arg0];
    *arg4 = mPeakMemory[arg0];
    if (arg5 != 0)
    {
        *arg5 = "";
    }
    return true;
}

GLResourcePool* glplatCreateResourcePool(
    const GLMemoryRequirement* requirements,
    unsigned long count, const char* name)
{
    unsigned long sizes[2] = { 0, 0 };
    for (unsigned long i = 0; i < count; ++i)
    {
        int pool = RealOrVirtual(requirements[i].mType);
        sizes[pool] += requirements[i].mSize;
    }

    return new (8, false)
        GLXResourcePool(sizes, name);
}

void* glplatResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource)
{
    return ((GLResourcePool*)resource)->Allocate(
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
        OSReport(sOutOfFrameMemoryFormat, szMemoryNames[memType]);
        nlBreak();
        return 0;
    }
    n_frame[i_frame][isLow] = newSize;
    return (void*)newTop;
}

unsigned long glx_GetFreeMemory()
{
    return glGetCurrentResourcePool()->GetFreeMemory();
}
