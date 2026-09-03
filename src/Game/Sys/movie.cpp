#include "Game/Sys/movie.h"

#include "RVL_SDK/thp/THPSimple.h"

#include "Game/ResourceInterface_802CC094.h"
#include "NL/gc/gcSwizzler.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxSwap.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFileGC.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <revolution/gx/GXMisc.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os/OSThread.h>

#include <string.h>

extern "C" ResourceInterface_802CC094* fn_802CC094();
extern "C" void fn_802CDF14(
    unsigned long texture, PlatTexture* platformTexture,
    MemoryAllocator* allocator);
extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" void fn_80372970(bool mono);

static THPVideoInfo videoInfo;
static PlatTexture* pTex[4];

static unsigned int lbl_806DFAC0 = -1;
static bool lbl_806DFAC4 = true;

static bool g_bActive;
static bool lbl_806E2419;
static bool lbl_806E241A;
static unsigned char* buffer;
static bool start;
static unsigned int lbl_806E2424;
static unsigned int lbl_806E2428;
static unsigned int lbl_806E242C;
static unsigned int lbl_806E2430;
static bool lbl_806E2434;
static unsigned int lbl_806E2438;
static bool g_bMovieMustStop;
static unsigned long resourceMarker;
static unsigned int lbl_806E2444;

extern "C" bool fn_80370E20()
{
    if (lbl_806E241A)
    {
        return true;
    }

    THPSimpleInit(1);
    lbl_806E241A = true;
    return true;
}

extern "C" bool fn_80370E64()
{
    THPSimpleQuit();
    lbl_806E241A = false;
    return true;
}

extern "C" void fn_80370E90(bool value)
{
    lbl_806DFAC4 = value;
}

bool MovieStart(
    const char* szFilename, bool bSound, bool bLoopMovie, bool bMono)
{
    if (g_bActive)
    {
        return false;
    }

    g_bMovieMustStop = false;

    char fileName[256];
    nlStrNCpy(fileName, szFilename, 256);
    nlToLower(fileName);

    if (!THPSimpleOpen(fileName))
    {
        return false;
    }

    THPSimpleGetVideoInfo(&videoInfo);
    ResourceInterface_802CC094* resourceInterface = fn_802CC094();
    MemoryAllocator* allocator = (MemoryAllocator*)resourceInterface;
    resourceMarker = resourceInterface->MarkResource();

    pTex[0] = glx_CreatePlatTexture(allocator);
    pTex[0]->Create(videoInfo.xSize, videoInfo.ySize, GXTex_I8,
        allocator, 1, false, false);
    fn_802CDF14(glGetTexture("movie"), pTex[0], allocator);

    pTex[1] = glx_CreatePlatTexture(allocator);
    pTex[1]->Create(videoInfo.xSize / 2, videoInfo.ySize / 2,
        GXTex_I8, allocator, 1, false, false);
    fn_802CDF14(glGetTexture("movie_u"), pTex[1], allocator);

    pTex[2] = glx_CreatePlatTexture(allocator);
    pTex[2]->Create(videoInfo.xSize / 2, videoInfo.ySize / 2,
        GXTex_I8, allocator, 1, false, false);
    fn_802CDF14(glGetTexture("movie_v"), pTex[2], allocator);

    unsigned long texSize = GCTextureSize(pTex[0]->m_Format,
        pTex[0]->m_Width, pTex[0]->m_Height, pTex[0]->m_Levels,
        (unsigned long)-1);
    memset(pTex[0]->m_SwizzledData, 0x10, texSize);

    texSize = GCTextureSize(pTex[1]->m_Format, pTex[1]->m_Width,
        pTex[1]->m_Height, pTex[1]->m_Levels, (unsigned long)-1);
    memset(pTex[1]->m_SwizzledData, 0x80, texSize);

    texSize = GCTextureSize(pTex[2]->m_Format, pTex[2]->m_Width,
        pTex[2]->m_Height, pTex[2]->m_Levels, (unsigned long)-1);
    memset(pTex[2]->m_SwizzledData, 0x80, texSize);

    pTex[0]->Prepare();
    pTex[1]->Prepare();
    pTex[2]->Prepare();
    GXInvalidateTexAll();

    buffer = (unsigned char*)nlMalloc(
        THPSimpleCalcNeedMemory(), 32, false);
    THPSimpleSetBuffer(buffer);

    unsigned int frame = glxGetFrameCount();
    lbl_806E2428 = frame;
    lbl_806E2438 = frame;
    lbl_806E242C = 0;

    if (!THPSimplePreLoad(bLoopMovie != false))
    {
        g_bActive = true;
        MovieStop();
        return false;
    }

    fn_80372970(bMono);
    lbl_806E2434 = false;
    start = true;
    g_bActive = true;
    lbl_806E2424 = 0;
    return true;
}

bool MovieStop()
{
    if (!g_bActive)
    {
        return false;
    }

    g_bActive = false;
    lbl_806E2419 = false;

    int lastFrame = THPSimpleGetTotalFrame() - 1;
    int currentFrame = (int)(unsigned int)lbl_806E2424;
    if ((unsigned int)currentFrame != (unsigned int)lastFrame)
    {
        fn_8004F594(3, "MOVIE did not finish playback.\n");
    }

    THPSimpleAudioStop();
    THPSimpleLoadStop();
    THPSimpleClose();

    if (buffer != 0)
    {
        nlFree(buffer);
    }

    buffer = 0;
    pTex[0] = 0;
    pTex[1] = 0;
    pTex[2] = 0;
    pTex[3] = 0;

    fn_802CC094()->ReleaseResource(resourceMarker);
    return true;
}

extern "C" void fn_80371254()
{
    ++lbl_806E2430;
}

bool MoviePlay()
{
    if (!g_bActive)
    {
        return false;
    }

    if (g_bMovieMustStop)
    {
        lbl_806E2434 = true;
        return false;
    }

    unsigned int frame = glxGetFrameCount();
    if (lbl_806E2444 != frame)
    {
        lbl_806E2444 = frame;
    }

    bool decode = false;
    if (lbl_806DFAC4)
    {
        if (frame >= lbl_806E2428)
        {
            decode = true;
        }
    }
    else if (lbl_806E2430 != lbl_806DFAC0)
    {
        decode = true;
        lbl_806DFAC0 = lbl_806E2430;
    }

    if (decode)
    {
        ++lbl_806E242C;

        if (lbl_806DFAC4)
        {
            GXSetDrawDone();
            GXFlush();
            nlServiceFileSystem();
            OSYieldThread();
            GXWaitDrawDone();
        }
        else
        {
            nlServiceFileSystem();
        }

        int error = THPSimpleDecode(0);
        if (error == 1 || error == 2)
        {
            lbl_806E2434 = true;
            return false;
        }

        if (start)
        {
            THPSimpleAudioStart();
            start = false;
        }

        ++lbl_806E2424;
        lbl_806E2428 = frame + 2;

        GXInvalidateTexAll();
        pTex[0]->Prepare();
        pTex[1]->Prepare();
        pTex[2]->Prepare();
    }
    else
    {
        nlServiceFileSystem();
    }

    return true;
}

extern "C" bool fn_803713C4()
{
    return g_bActive;
}

extern "C" bool fn_803713CC()
{
    return lbl_806E2434;
}

extern "C" void fn_803713D4()
{
    lbl_806E2434 = false;
}

extern "C" unsigned int fn_803713E0()
{
    return lbl_806E2424;
}
