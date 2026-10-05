#include "Game/Sys/movie.h"
#include "Game/Sys/debug.h"

#include "RVL_SDK/thp/THPSimple.h"

#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/gc/gcSwizzler.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxSwap.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFileGC.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <revolution/gx/GXMisc.h>
#include <revolution/gx/GXTexture.h>
#include <revolution/os/OSThread.h>

#include <string.h>
#include "NL/nlstring_tmpl.h"
#include "NL/gl/glTexture.h"

static THPVideoInfo videoInfo;
static PlatTexture* pTex[4];

static unsigned int g_uLastDecodeRenderTick = -1;
static bool g_bSyncedDecode = true;

static bool g_bActive;
static bool lbl_806E2419;
static bool g_bTHPInitialized;
static unsigned char* buffer;
static bool start;
static unsigned int g_uCurrentFrame;
static unsigned int g_uNextDecodeFrame;
static unsigned int g_uDecodeCount;
static unsigned int g_uRenderTickCount;
static bool g_bFinished;
static unsigned int g_uStartFrame;
static bool g_bMovieMustStop;
static unsigned long resourceMarker;
static unsigned int g_uLastPlayFrame;

bool MovieInit()
{
    if (g_bTHPInitialized)
    {
        return true;
    }

    THPSimpleInit(1);
    g_bTHPInitialized = true;
    return true;
}

bool MovieQuit()
{
    THPSimpleQuit();
    g_bTHPInitialized = false;
    return true;
}

void SetSyncedDecode(bool value)
{
    g_bSyncedDecode = value;
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
    GLResourcePool* resourceInterface = glGetCurrentResourcePool();
    MemoryAllocator* allocator = (MemoryAllocator*)resourceInterface;
    resourceMarker = resourceInterface->MarkResource();

    pTex[0] = glx_CreatePlatTexture(allocator);
    pTex[0]->Create(videoInfo.xSize, videoInfo.ySize, GXTex_I8,
        allocator, 1, false, false);
    glRegisterTexture(glGetTexture("movie"), pTex[0], allocator);

    pTex[1] = glx_CreatePlatTexture(allocator);
    pTex[1]->Create(videoInfo.xSize / 2, videoInfo.ySize / 2,
        GXTex_I8, allocator, 1, false, false);
    glRegisterTexture(glGetTexture("movie_u"), pTex[1], allocator);

    pTex[2] = glx_CreatePlatTexture(allocator);
    pTex[2]->Create(videoInfo.xSize / 2, videoInfo.ySize / 2,
        GXTex_I8, allocator, 1, false, false);
    glRegisterTexture(glGetTexture("movie_v"), pTex[2], allocator);

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
    g_uNextDecodeFrame = frame;
    g_uStartFrame = frame;
    g_uDecodeCount = 0;

    if (!THPSimplePreLoad(bLoopMovie != false))
    {
        g_bActive = true;
        MovieStop();
        return false;
    }

    fn_80372970(bMono);
    g_bFinished = false;
    start = true;
    g_bActive = true;
    g_uCurrentFrame = 0;
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
    int currentFrame = (int)(unsigned int)g_uCurrentFrame;
    if ((unsigned int)currentFrame != (unsigned int)lastFrame)
    {
        tDebugPrintManager::Print(DC_RENDER, "MOVIE did not finish playback.\n");
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

    glGetCurrentResourcePool()->ReleaseResource(resourceMarker);
    return true;
}

void MovieRenderTick()
{
    ++g_uRenderTickCount;
}

bool MoviePlay()
{
    if (!g_bActive)
    {
        return false;
    }

    if (g_bMovieMustStop)
    {
        g_bFinished = true;
        return false;
    }

    unsigned int frame = glxGetFrameCount();
    if (g_uLastPlayFrame != frame)
    {
        g_uLastPlayFrame = frame;
    }

    bool decode = false;
    if (g_bSyncedDecode)
    {
        if (frame >= g_uNextDecodeFrame)
        {
            decode = true;
        }
    }
    else if (g_uRenderTickCount != g_uLastDecodeRenderTick)
    {
        decode = true;
        g_uLastDecodeRenderTick = g_uRenderTickCount;
    }

    if (decode)
    {
        ++g_uDecodeCount;

        if (g_bSyncedDecode)
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
            g_bFinished = true;
            return false;
        }

        if (start)
        {
            THPSimpleAudioStart();
            start = false;
        }

        ++g_uCurrentFrame;
        g_uNextDecodeFrame = frame + 2;

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

bool IsMovieActive()
{
    return g_bActive;
}

bool IsMovieFinished()
{
    return g_bFinished;
}

void ClearMovieFinished()
{
    g_bFinished = false;
}

unsigned int GetMovieFrame()
{
    return g_uCurrentFrame;
}
