#include <revolution/gx.h>

#include "NL/gl/glTexture.h"
#include "NL/glx/tu_8036D894.h"

#include "NL/gc/gcSwizzler.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

static TargetPlatform_8036DE50* lbl_806E23D0;
static TargetPlatform_8036DE50* lbl_806E23D4;
static GXColor lbl_806E23D8;
static unsigned char clearz_mem[320 * 224] ATTRIBUTE_ALIGN(32);

extern "C" TargetPlatform_8036DE50* fn_8036D894()
{
    return lbl_806E23D0;
}

extern "C" void fn_8036D89C()
{
    TargetInfo_8036DE50 info;
    info.width = 640;
    info.height = 448;
    info.format = 0;
    info.unknown18 = 7;
    info.unknown1C = 3;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    lbl_806E23D0 = new (8, false) TargetPlatform_8036DE50(&info);
    lbl_806E23D4 = 0;
}

void TargetPlatform_8036DE50::fn_8036D9A0(unsigned long mode)
{
    lbl_806E23D4 = this;
}

void TargetPlatform_8036DE50::fn_8036D9A8()
{
}

void TargetPlatform_8036DE50::fn_8036D9AC(unsigned long hash)
{
    mUnidentified004 = hash;
    MemoryAllocator* allocator = (MemoryAllocator*)fn_802CC094();
    if (mUnidentified020 == 6)
    {
        mUnidentified008 = glResourceAlloc(GXGetTexBufferSize(
                                               mUnidentified010, mUnidentified014, GX_CTF_A8, false, 0),
            GLM_Target,
            allocator);
        mUnidentified00C = glx_CreatePlatTexture(allocator);
        mUnidentified00C->CreateWithMemory(mUnidentified010, mUnidentified014, GXTex_A8, 1, mUnidentified008);
        mUnidentified00C->Prepare();
        mUnidentified024 = GX_CTF_A8;
    }
    else if (mUnidentified020 == 7)
    {
        mUnidentified008 = glResourceAlloc(GXGetTexBufferSize(
                                               mUnidentified010, mUnidentified014, GX_TF_IA8, false, 0),
            GLM_Target,
            allocator);
        mUnidentified00C = glx_CreatePlatTexture(allocator);
        mUnidentified00C->CreateWithMemory(mUnidentified010, mUnidentified014, GXTex_IA8, 1, mUnidentified008);
        mUnidentified00C->Prepare();
        mUnidentified024 = GX_TF_IA8;
    }
    else
    {
        eGXTextureFormat format;
        if (mUnidentified020 == 0)
        {
            mUnidentified024 = GX_TF_RGB5A3;
            format = GXTex_RGB5A3;
        }
        else if (mUnidentified020 == 5)
        {
            mUnidentified024 = GX_TF_RGBA8;
            format = GXTex_RGBA8;
        }
        else
        {
            mUnidentified024 = GX_TF_RGB565;
            format = GXTex_RGB565;
        }
        mUnidentified008 = glResourceAlloc(GXGetTexBufferSize(
                                               mUnidentified010, mUnidentified014, (GXTexFmt)mUnidentified024, false, 0),
            GLM_Target,
            allocator);
        mUnidentified00C = glx_CreatePlatTexture(allocator);
        mUnidentified00C->CreateWithMemory(mUnidentified010, mUnidentified014, format, 1, mUnidentified008);
        mUnidentified00C->Prepare();
    }
    nlZeroMemory(mUnidentified00C->m_SwizzledData,
        GCTextureSize(mUnidentified00C->m_Format, mUnidentified00C->m_Width, mUnidentified00C->m_Height, mUnidentified00C->m_Levels, -1));
    glRegisterTexture(mUnidentified004, mUnidentified00C, allocator);
}

void TargetPlatform_8036DE50::fn_8036DBA8()
{
}

void TargetPlatform_8036DE50::fn_8036DBAC(unsigned long hash)
{
}

void TargetPlatform_8036DE50::fn_8036DBB0()
{
    fn_8036DBCC(mUnidentified018, mUnidentified019, false);
}

void TargetPlatform_8036DE50::fn_8036DBCC(bool flag0, bool flag1, bool flag2)
{
    bool flag3 = flag0 && mUnidentified018;
    bool flag4 = flag1 && mUnidentified019;
    GXColor clearColour = { mUnidentified01A.c[0], mUnidentified01A.c[1], mUnidentified01A.c[2], mUnidentified01A.c[3] };
    bool colorUpdate = gxSetColourUpdate(flag3);
    bool alphaUpdate = gxSetAlphaUpdate(flag3);
    gxSaveZMode();
    gxSetZMode(false, GX_LEQUAL, flag4);
    GXSetTexCopySrc(0, 0, 640, 448);
    GXSetTexCopyDst(320, 224, GX_CTF_R8, true);
    GXSetCopyClear(clearColour, 0xFFFFFF);
    GXCopyTex(clearz_mem, true);
    GXSetCopyClear(lbl_806E23D8, 0xFFFFFF);
    gxSetColourUpdate(colorUpdate);
    gxSetAlphaUpdate(alphaUpdate);
    gxRestoreZMode();
}

void TargetPlatform_8036DE50::fn_8036DD14(bool flag0, bool flag1)
{
    unsigned long frame = glGetCurrentFrame();
    if (mUnidentified028 != frame)
        mUnidentified028 = frame;
    else
        GXInvalidateTexAll();

    PlatformViewport viewport = *fn_80369A30();
    bool halfSize = mUnidentified010 == viewport.width / 2
                 && mUnidentified014 == viewport.height / 2;
    bool colorUpdate = gxSetColourUpdate(true);
    bool alphaUpdate = gxSetAlphaUpdate(true);
    gxSaveZMode();
    gxSetZMode(false, GX_LEQUAL, true);
    if (flag1)
    {
        gxSetColourUpdate(false);
        gxSetZMode(false, GX_LEQUAL, false);
    }
    GXSetTexCopySrc(viewport.x, viewport.y, viewport.width, viewport.height);
    GXSetTexCopyDst(mUnidentified010, mUnidentified014, (GXTexFmt)mUnidentified024, halfSize);
    GXCopyTex(mUnidentified00C->m_SwizzledData, flag0);
    GXPixModeSync();
    gxSetColourUpdate(colorUpdate);
    gxSetAlphaUpdate(alphaUpdate);
    gxRestoreZMode();
}

extern "C" TargetPlatform_8036DE50* fn_8036DE50(const TargetInfo_8036DE50* targetInfo)
{
    return new (8, false) TargetPlatform_8036DE50(targetInfo);
}

extern "C" void fn_8036DF24(TargetPlatform_8036DE50* target, bool flag0, bool flag1)
{
    target->fn_8036DD14(flag0, flag1);
}

TargetPlatform_8036DE50::~TargetPlatform_8036DE50()
{
}
