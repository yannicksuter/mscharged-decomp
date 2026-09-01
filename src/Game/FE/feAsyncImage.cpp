#include "Game/FE/feAsyncImage.h"

#include "Game/FE/feResourceManager.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlImageInstance.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glTexture.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

extern unsigned int nlDefaultSeed;
extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

inline bool AsyncImage::CanSwapTextures() const
{
    bool res;

    if (mLoadState != LS_LOAD_COMPLETE)
    {
        res = false;
    }
    else
    {
        if (mImageInstance != 0)
        {
            res = false;
            if (mImageInstance != 0 && mImageInstance->m_pTextureResource->m_bValid && glTextureLoad(mTextureHandle) != 0)
            {
                res = true;
            }
        }
        else
        {
            res = false;
            if (glTextureLoad(mUnidentified14) != 0 && glTextureLoad(mTextureHandle) != 0)
            {
                res = true;
            }
        }
    }
    return res;
}

inline void AsyncImage::SwapTextures()
{
    glFinish();
    if (mImageInstance != 0)
    {
        glTextureReplace(mImageInstance->m_pTextureResource->GetTextureHandle(), m_loadBuffer, mTextureSize);
    }
    else
    {
        glTextureReplace(mUnidentified14, m_loadBuffer, mTextureSize);
    }
    glDiscardFrame(1);
}

inline void AsyncImage::FreeLoadBuffer()
{
    if (m_loadBuffer != 0)
    {
        delete[] (u8*)m_loadBuffer;
        m_loadBuffer = 0;
    }
}

void AsyncImage::TextureLoadComplete(void* buffer, unsigned long size, unsigned long userData)
{
    AsyncImage* self = (AsyncImage*)userData;
    self->mLoadState = LS_LOAD_COMPLETE;
}

void AsyncImage::fn_801BF4D4(void* arg0, unsigned long arg1, unsigned long userData)
{
    AsyncImage* self = (AsyncImage*)userData;
    self->mLoadState = LS_UNIDENTIFIED_1;
}

bool AsyncImage::Update(bool autoswap)
{
    if (mImageInstance == 0 && mUnidentified14 == 0)
    {
        return false;
    }

    bool res = false;
    if (mLoadState == LS_UNIDENTIFIED_1)
    {
        BundleFileDirectoryEntry info;
        mBundleFile->GetFileInfo(mUnidentifiedPath, &info, true);

        if (m_loadBuffer == 0)
        {
            AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
            CurrentAllocator = &VirtualAllocator;
            m_loadBuffer = (u8*)nlMalloc(info.m_length, 0x20, true);
            --AllocatorStackDepth;
            AllocatorStack[AllocatorStackDepth] = 0;
            CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
            mTextureSize = info.m_length;
        }

        mBundleFile->ReadFileAsync(mUnidentifiedPath, m_loadBuffer, mTextureSize, &AsyncImage::TextureLoadComplete, (unsigned long)this);
        mLoadState = LS_ISSUED_LOAD;
    }

    if (mLoadState == LS_LOAD_COMPLETE)
    {
        if (mImageInstance != 0)
        {
            if (mTextureHandle != mImageInstance->m_pTextureResource->GetTextureHandle() && glTextureLoad(mTextureHandle) == 0)
            {
                glTextureAdd(mTextureHandle, m_loadBuffer, mTextureSize, FEResourceManager::s_pInstance->fn_802FDD84());
                mImageInstance->m_pTextureResource->fn_8030009C(mTextureHandle);
            }
        }
        else if (glTextureLoad(mTextureHandle) == 0)
        {
            glTextureAdd(mTextureHandle, m_loadBuffer, mTextureSize, FEResourceManager::s_pInstance->fn_802FDD84());
        }
    }

    if (mLoadState == LS_LOAD_COMPLETE && autoswap)
    {
        if (CanSwapTextures())
        {
            SwapTextures();
            FreeLoadBuffer();
            res = true;
            mLoadState = LS_IDLE;
        }
    }
    return res;
}

void AsyncImage::QueueLoad(const char* path, bool isblocking)
{
    if (mLoadState != LS_ISSUED_LOAD)
    {
        nlStrNCpy(mUnidentifiedPath, path, sizeof(mUnidentifiedPath));
        if (mLoadState != LS_UNIDENTIFIED_1 && mLoadState != LS_UNIDENTIFIED_0)
        {
            mLoadState = LS_UNIDENTIFIED_1;
        }
    }
}

AsyncImage::~AsyncImage()
{
    while (mLoadState == LS_ISSUED_LOAD || mLoadState == LS_UNIDENTIFIED_0)
    {
        nlServiceFileSystem();
    }

    mBundleFile->Close();
    delete mBundleFile;
    FreeLoadBuffer();
}

AsyncImage::AsyncImage(const char* filename, const char* texturename)
{
    mBundleFile = 0;
    mImageInstance = 0;
    m_loadBuffer = 0;
    mUnidentified14 = 0;
    mTextureSize = 0;
    mLoadState = LS_UNIDENTIFIED_0;

    mBundleFile = new (nlMalloc(sizeof(BundleFile), 0x20, 1)) BundleFile();
    mBundleFile->OpenAsync(filename, &AsyncImage::fn_801BF4D4, (unsigned long)this, false);

    int hash;
    if (texturename != 0)
    {
        hash = nlStringHash(texturename);
    }
    else
    {
        hash = nlRandom(0xFFFFFFFF, &nlDefaultSeed);
    }
    mTextureHandle = hash;
}
