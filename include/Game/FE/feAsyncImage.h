#ifndef GAME_FE_FE_ASYNC_IMAGE_H
#define GAME_FE_FE_ASYNC_IMAGE_H

#include "NL/nlBundleFile.h"
#include "types.h"

class TLImageInstance;

enum LoadState
{
    LS_OPENING_BUNDLE = 0,
    LS_READY_TO_LOAD = 1,
    LS_ISSUED_LOAD = 2,
    LS_LOAD_COMPLETE = 3,
    LS_IDLE = 4,
};

class AsyncImage
{
public:
    AsyncImage(const char* filename, const char* texturename);
    virtual ~AsyncImage();

    void QueueLoad(const char* path, bool isblocking);
    bool Update(bool autoswap);

    void SetImageInstance(TLImageInstance* instance)
    {
        mImageInstance = instance;
    }

    bool CanSwapTextures() const;
    void SwapTextures();
    void FreeLoadBuffer();

    static void BundleOpenComplete(void* buffer, unsigned long size, unsigned long userData);
    static void TextureLoadComplete(void* buffer, unsigned long size, unsigned long userData);

    /* 0x04 */ BundleFile* mBundleFile;
    /* 0x08 */ TLImageInstance* mImageInstance;
    /* 0x0C */ void* m_loadBuffer;
    /* 0x10 */ unsigned long mTextureHandle;
    /* 0x14 */ unsigned long mTargetTextureHandle;
    /* 0x18 */ unsigned long mTextureSize;
    /* 0x1C */ LoadState mLoadState;
    /* 0x20 */ char mLoadPath[0x80];
}; // size 0xA0

#endif // GAME_FE_FE_ASYNC_IMAGE_H
