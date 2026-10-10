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
#include "Game/SharedStaticStorage.h"

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

static unsigned char sDumpFrameMemory;
static unsigned long sFrameMemorySizes[2];
static int sCurrentFrame;
static bool sFrameMemoryInitialized;
static GLInventory::ModelReleaseCallback sModelReleaseCallback;

static unsigned long sFrameMemory[2][2];
static unsigned long sFrameMemoryUsed[2][2];

static inline int GetMemoryPoolIndex(eGLMemory memType)
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

    sFrameMemory[0][0] = pMem;
    sFrameMemory[1][0] = pMem + frameMemSize1;

    CurrentAllocator = &VirtualAllocator;
    pMem = (unsigned long)nlMalloc(frameMemSize2 * 2, 32, false);
    if (pMem == 0)
    {
        return false;
    }

    sFrameMemory[0][1] = pMem;
    sFrameMemory[1][1] = pMem + frameMemSize2;
    CurrentAllocator = allocator;

    sCurrentFrame = 0;
    sFrameMemoryUsed[1][0] = 0;
    sFrameMemoryUsed[0][0] = 0;
    sFrameMemoryUsed[1][1] = 0;
    sFrameMemoryUsed[0][1] = 0;
    sFrameMemorySizes[0] = frameMemSize1;
    sFrameMemorySizes[1] = frameMemSize2;
    sFrameMemoryInitialized = true;
    return true;
}

void glplatFrameAllocNextFrame()
{
    if (sDumpFrameMemory)
    {
        tDebugPrintManager::Print(DC_GLPLAT, sFrameMemoryUsedFormat,
            sFrameMemoryUsed[sCurrentFrame][0] >> 10, sFrameMemoryUsed[sCurrentFrame][1] >> 10);
        tDebugPrintManager::Print(DC_GLPLAT, sFrameMemoryFreeFormat,
            (sFrameMemorySizes[0] - sFrameMemoryUsed[sCurrentFrame][0]) >> 10,
            (sFrameMemorySizes[1] - sFrameMemoryUsed[sCurrentFrame][1]) >> 10);
        sDumpFrameMemory = false;
    }

    int newFrame = sCurrentFrame ^ 1;
    sCurrentFrame = newFrame;
    sFrameMemoryUsed[newFrame][0] = 0;
    sFrameMemoryUsed[newFrame][1] = 0;
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
    int pool = GetMemoryPoolIndex(memType);
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
    GLResourceMarker* marker
        = (GLResourceMarker*)glResourceAlloc(
            sizeof(GLResourceMarker), GLM_Header, this);
    marker->mUsedMemory[0] = value0;
    marker->mUsedMemory[1] = value1;
    marker->mLevel = m_level;
    marker->mPool = this;
    m_inventory->ResourceMark();
    m_level++;
    return (unsigned long)marker;
}

void GLXResourcePool::ReleaseResource(
    unsigned long value)
{
    GLResourceMarker* marker
        = (GLResourceMarker*)value;
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
    unsigned long pool, const char** poolName, unsigned long* totalBytes,
    unsigned long* freeBytes, unsigned long* peakBytes, const char** description)
{
    if (pool > 1)
    {
        return false;
    }
    *poolName = pool != 0 ? sMEM2Name : sMEM1Name;
    *totalBytes = mPoolSizes[pool];
    *freeBytes = mPoolSizes[pool] - mUsedMemory[pool];
    *peakBytes = mPeakMemory[pool];
    if (description != 0)
    {
        *description = "";
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
        int pool = GetMemoryPoolIndex(requirements[i].mType);
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
    unsigned long pool = GetMemoryPoolIndex(memType);
    unsigned long newTop
        = (sFrameMemory[sCurrentFrame][pool] + sFrameMemoryUsed[sCurrentFrame][pool] + 31)
        & ~31u;
    unsigned long newSize = size + (newTop - sFrameMemory[sCurrentFrame][pool]);
    if (newSize > sFrameMemorySizes[pool])
    {
        OSReport(sOutOfFrameMemoryFormat, szMemoryNames[memType]);
        nlBreak();
        return 0;
    }
    sFrameMemoryUsed[sCurrentFrame][pool] = newSize;
    return (void*)newTop;
}

unsigned long glx_GetFreeMemory()
{
    return glGetCurrentResourcePool()->GetFreeMemory();
}
