#ifndef NL_GLX_GLXTARGET_H
#define NL_GLX_GLXTARGET_H

#include "NL/gl/glTarget.h"
#include "NL/nlColour.h"

class PlatTexture;

class GLXTarget
{
public:
    GLXTarget(const GLTargetInfo* targetInfo)
        : mCopyFormat(-1)
        , mLastCopyFrame(0)
    {
        mWidth = targetInfo->width;
        mHeight = targetInfo->height;
        mFormat = targetInfo->format;
        mClearColourEnabled = (targetInfo->unknown18 & 1) != 0;
        mClearDepthEnabled = (targetInfo->unknown18 & 4) != 0;
        mClearDepthEnabled |= targetInfo->unknown1C != 0;
        nlColourSet(mClearColour, targetInfo->colour[0], targetInfo->colour[1], targetInfo->colour[2], targetInfo->colour[3]);
        mTextureData = 0;
        mTexture = 0;
    }

    virtual ~GLXTarget();
    virtual void Activate(unsigned long mode);
    virtual void UnidentifiedVirtual10();
    virtual void CreateTexture(unsigned long hash);
    virtual void DestroyTexture(unsigned long hash);
    virtual void ClearDefault();
    virtual void ClearBuffers(bool clearColourBuffer, bool clearDepthBuffer, bool unknown);
    virtual void UnidentifiedVirtual28();

    void CopyToTexture(bool flag0, bool flag1);

    /* 0x04 */ unsigned long mTextureHash;
    /* 0x08 */ void* mTextureData;
    /* 0x0C */ PlatTexture* mTexture;
    /* 0x10 */ unsigned long mWidth;
    /* 0x14 */ unsigned long mHeight;
    /* 0x18 */ bool mClearColourEnabled;
    /* 0x19 */ bool mClearDepthEnabled;
    /* 0x1A */ nlColour mClearColour;
    /* 0x20 */ int mFormat;
    /* 0x24 */ int mCopyFormat;
    /* 0x28 */ unsigned long mLastCopyFrame;
}; // size: 0x2C

GLXTarget* glplatGetBackBufferTarget();
void glxInitTargets();
GLXTarget* glplatCreateTarget(const GLTargetInfo* targetInfo);
void glplatCopyTargetToTexture(GLXTarget* target, bool flag0, bool flag1);

#endif // NL_GLX_GLXTARGET_H
