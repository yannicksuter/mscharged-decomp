#ifndef NL_GL_GL_TEXTURE_MANAGER_H
#define NL_GL_GL_TEXTURE_MANAGER_H

#include "types.h"

class GLTextureAnim;
class PlatTexture;
struct glTextureBinding;

struct glTextureIndexQueue
{
    glTextureIndexQueue(u16* data, unsigned long capacity)
    {
        mHead = 0;
        mCount = 0;
        mCapacity = capacity;
        mBuffer = data;
    }

    void AddEnd(const u16& value)
    {
        u16* entry;
        if (mCount >= mCapacity)
        {
            entry = 0;
        }
        else
        {
            entry = mBuffer + (mHead + mCount) % mCapacity;
            ++mCount;
        }
        *entry = value;
    }

    u16 RemoveStart()
    {
        u16* entry;
        if (mCount == 0)
        {
            entry = mBuffer + (mHead & mCapacity);
        }
        else
        {
            --mCount;
            entry = mBuffer + mHead;
            ++mHead;
            if (mHead % mCapacity == 0)
            {
                mHead = 0;
            }
        }
        return *entry;
    }

    /* 0x00 */ u16* mBuffer;
    /* 0x04 */ u32 mHead;
    /* 0x08 */ u32 mCount;
    /* 0x0C */ u32 mCapacity;
};

class glTextureManager
{
public:
    explicit glTextureManager(unsigned long count);
    unsigned long GetTextureIndex(unsigned long texture);
    void ResolveTextureIndex(glTextureBinding* texture);
    PlatTexture* GetTextureAtIndex(const unsigned long* texture);
    PlatTexture* GetTexture(glTextureBinding* texture);
    void RegisterTexture(PlatTexture* texture);
    void RegisterTextureAnim(GLTextureAnim* anim);
    void RefreshTextureAnim(GLTextureAnim* anim);

    /* 0x00 */ u32 mCapacity;
    /* 0x04 */ PlatTexture** mTextures;
    /* 0x08 */ glTextureIndexQueue* mFreeIndices;
    /* 0x0C */ u16* mIndexBuffer;
}; // size: 0x10

extern glTextureManager* gTextureManager;

glTextureManager* glGetTextureManager();

#endif // NL_GL_GL_TEXTURE_MANAGER_H
