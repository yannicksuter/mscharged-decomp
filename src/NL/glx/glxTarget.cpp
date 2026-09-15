#include <revolution/gx.h>
#include "NL/gl/glTexture.h"

#include "NL/gl/glTexture.h"
#include "NL/glx/glxTarget.h"

#include "NL/gc/gcSwizzler.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/gl/glTexture.h"

static GLXTarget* sBackBufferTarget;
static GLXTarget* sCurrentTarget;
static GXColor sDefaultCopyClearColour;
static unsigned char clearz_mem[320 * 224] ATTRIBUTE_ALIGN(32);

GLXTarget* glplatGetBackBufferTarget()
{
    return sBackBufferTarget;
}

void glxInitTargets()
{
    GLTargetInfo info;
    info.width = 640;
    info.height = 448;
    info.unknown10 = GLTargetInfoMode10_0;
    info.format = GLTargetFormat_0;
    info.colour[0] = 0;
    info.colour[1] = 0;
    info.colour[2] = 0;
    info.colour[3] = 0;
    info.unknown18 = 7;
    info.unknown1C = 3;
    sBackBufferTarget = new (8, false) GLXTarget(&info);
    sCurrentTarget = 0;
}

void GLXTarget::Activate(unsigned long mode)
{
    sCurrentTarget = this;
}

void GLXTarget::UnidentifiedVirtual10()
{
}

void GLXTarget::CreateTexture(unsigned long hash)
{
    mTextureHash = hash;
    MemoryAllocator* allocator = (MemoryAllocator*)glGetCurrentResourcePool();
    if (mFormat == 6)
    {
        mTextureData = glResourceAlloc(GXGetTexBufferSize(
                                               mWidth, mHeight, GX_CTF_A8, false, 0),
            GLM_Target,
            allocator);
        mTexture = glx_CreatePlatTexture(allocator);
        mTexture->CreateWithMemory(mWidth, mHeight, GXTex_A8, 1, mTextureData);
        mTexture->Prepare();
        mCopyFormat = GX_CTF_A8;
    }
    else if (mFormat == 7)
    {
        mTextureData = glResourceAlloc(GXGetTexBufferSize(
                                               mWidth, mHeight, GX_TF_IA8, false, 0),
            GLM_Target,
            allocator);
        mTexture = glx_CreatePlatTexture(allocator);
        mTexture->CreateWithMemory(mWidth, mHeight, GXTex_IA8, 1, mTextureData);
        mTexture->Prepare();
        mCopyFormat = GX_TF_IA8;
    }
    else
    {
        eGXTextureFormat format;
        if (mFormat == 0)
        {
            mCopyFormat = GX_TF_RGB5A3;
            format = GXTex_RGB5A3;
        }
        else if (mFormat == 5)
        {
            mCopyFormat = GX_TF_RGBA8;
            format = GXTex_RGBA8;
        }
        else
        {
            mCopyFormat = GX_TF_RGB565;
            format = GXTex_RGB565;
        }
        mTextureData = glResourceAlloc(GXGetTexBufferSize(
                                               mWidth, mHeight, (GXTexFmt)mCopyFormat, false, 0),
            GLM_Target,
            allocator);
        mTexture = glx_CreatePlatTexture(allocator);
        mTexture->CreateWithMemory(mWidth, mHeight, format, 1, mTextureData);
        mTexture->Prepare();
    }
    nlZeroMemory(mTexture->m_SwizzledData,
        GCTextureSize(mTexture->m_Format, mTexture->m_Width, mTexture->m_Height, mTexture->m_Levels, -1));
    glRegisterTexture(mTextureHash, mTexture, allocator);
}

void GLXTarget::UnidentifiedVirtual28()
{
}

void GLXTarget::DestroyTexture(unsigned long hash)
{
}

void GLXTarget::ClearDefault()
{
    ClearBuffers(mClearColourEnabled, mClearDepthEnabled, false);
}

void GLXTarget::ClearBuffers(bool clearColourBuffer, bool clearDepthBuffer, bool unknown)
{
    bool clearColourEnabled = clearColourBuffer && mClearColourEnabled;
    bool clearDepthEnabled = clearDepthBuffer && mClearDepthEnabled;
    GXColor clearColour = { mClearColour.c[0], mClearColour.c[1], mClearColour.c[2], mClearColour.c[3] };
    bool colorUpdate = gxSetColourUpdate(clearColourEnabled);
    bool alphaUpdate = gxSetAlphaUpdate(clearColourEnabled);
    gxSaveZMode();
    gxSetZMode(false, GX_LEQUAL, clearDepthEnabled);
    GXSetTexCopySrc(0, 0, 640, 448);
    GXSetTexCopyDst(320, 224, GX_CTF_R8, true);
    GXSetCopyClear(clearColour, 0xFFFFFF);
    GXCopyTex(clearz_mem, true);
    GXSetCopyClear(sDefaultCopyClearColour, 0xFFFFFF);
    gxSetColourUpdate(colorUpdate);
    gxSetAlphaUpdate(alphaUpdate);
    gxRestoreZMode();
}

void GLXTarget::CopyToTexture(bool flag0, bool flag1)
{
    unsigned long frame = glGetCurrentFrame();
    if (mLastCopyFrame != frame)
        mLastCopyFrame = frame;
    else
        GXInvalidateTexAll();

    const PlatformViewport* viewport = glplatGetViewport();
    int srcLeft = viewport->x;
    int srcTop = viewport->y;
    int srcWidth = viewport->width;
    int srcHeight = viewport->height;
    bool halfSize = mWidth == srcWidth / 2
                 && mHeight == srcHeight / 2;
    bool colorUpdate = gxSetColourUpdate(true);
    bool alphaUpdate = gxSetAlphaUpdate(true);
    gxSaveZMode();
    gxSetZMode(false, GX_LEQUAL, true);
    if (flag1)
    {
        gxSetColourUpdate(false);
        gxSetZMode(false, GX_LEQUAL, false);
    }
    GXSetTexCopySrc(srcLeft, srcTop, srcWidth, srcHeight);
    GXSetTexCopyDst(mWidth, mHeight, (GXTexFmt)mCopyFormat, halfSize);
    GXCopyTex(mTexture->m_SwizzledData, flag0);
    GXPixModeSync();
    gxSetColourUpdate(colorUpdate);
    gxSetAlphaUpdate(alphaUpdate);
    gxRestoreZMode();
}

GLXTarget* glplatCreateTarget(const GLTargetInfo* targetInfo)
{
    return new (8, false) GLXTarget(targetInfo);
}

void glplatCopyTargetToTexture(GLXTarget* target, bool flag0, bool flag1)
{
    target->CopyToTexture(flag0, flag1);
}

GLXTarget::~GLXTarget()
{
}
