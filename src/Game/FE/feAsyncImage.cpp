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
#include "NL/nlstring_tmpl.h"

extern unsigned int nlDefaultSeed;

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
            if (glTextureLoad(mTargetTextureHandle) != 0 && glTextureLoad(mTextureHandle) != 0)
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
        glTextureReplace(mTargetTextureHandle, m_loadBuffer, mTextureSize);
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

void AsyncImage::BundleOpenComplete(void* buffer, unsigned long size, unsigned long userData)
{
    AsyncImage* self = (AsyncImage*)userData;
    self->mLoadState = LS_READY_TO_LOAD;
}

bool AsyncImage::Update(bool autoswap)
{
    if (mImageInstance == 0 && mTargetTextureHandle == 0)
    {
        return false;
    }

    bool res = false;
    if (mLoadState == LS_READY_TO_LOAD)
    {
        BundleFileDirectoryEntry info;
        mBundleFile->GetFileInfo(mLoadPath, &info, true);

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

        mBundleFile->ReadFileAsync(mLoadPath, m_loadBuffer, mTextureSize, &AsyncImage::TextureLoadComplete, (unsigned long)this);
        mLoadState = LS_ISSUED_LOAD;
    }

    if (mLoadState == LS_LOAD_COMPLETE)
    {
        if (mImageInstance != 0)
        {
            if (mTextureHandle != mImageInstance->m_pTextureResource->GetTextureHandle() && glTextureLoad(mTextureHandle) == 0)
            {
                glTextureAdd(mTextureHandle, m_loadBuffer, mTextureSize, FEResourceManager::s_pInstance->GetResourcePool());
                mImageInstance->m_pTextureResource->SetTextureHandle(mTextureHandle);
            }
        }
        else if (glTextureLoad(mTextureHandle) == 0)
        {
            glTextureAdd(mTextureHandle, m_loadBuffer, mTextureSize, FEResourceManager::s_pInstance->GetResourcePool());
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
        nlStrNCpy(mLoadPath, path, sizeof(mLoadPath));
        if (mLoadState != LS_READY_TO_LOAD && mLoadState != LS_OPENING_BUNDLE)
        {
            mLoadState = LS_READY_TO_LOAD;
        }
    }
}

AsyncImage::~AsyncImage()
{
    while (mLoadState == LS_ISSUED_LOAD || mLoadState == LS_OPENING_BUNDLE)
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
    mTargetTextureHandle = 0;
    mTextureSize = 0;
    mLoadState = LS_OPENING_BUNDLE;

    mBundleFile = new (nlMalloc(sizeof(BundleFile), 0x20, 1)) BundleFile();
    mBundleFile->OpenAsync(filename, &AsyncImage::BundleOpenComplete, (unsigned long)this, false);

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
