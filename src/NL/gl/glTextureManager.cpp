#include "NL/gl/glTextureManager.h"

#include "Game/GL/GLTextureAnim.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"

#include <string.h>

glTextureManager::glTextureManager(unsigned long count)
{
    mCapacity = count;
    unsigned long size = count * sizeof(PlatTexture*);
    mTextures = (PlatTexture**)nlMalloc(size, 8, false);
    memset(mTextures, 0, size);
    mIndexBuffer = new (8, false) u16[count];
    mFreeIndices = new (8, false) glTextureIndexQueue(mIndexBuffer, count);
    for (u32 i = 0; i < count; ++i)
    {
        mFreeIndices->AddEnd((u16)i);
    }
}

unsigned long glTextureManager::GetTextureIndex(unsigned long texture)
{
    GLTextureAnim* anim = fn_802D0758(texture);
    if (anim != 0)
    {
        return anim->m_unk_0x18;
    }
    PlatTexture* platformTexture = glx_GetTex(texture);
    if (platformTexture == 0)
    {
        return 0xFFFF;
    }
    return platformTexture->unknown0E;
}

void glTextureManager::ResolveTextureIndex(UnidentifiedTextureState* texture)
{
    if (texture->textureIndex == 0xFFFF)
    {
        texture->textureIndex = GetTextureIndex(texture->texture);
    }
}

PlatTexture* glTextureManager::GetTextureAtIndex(const unsigned long* texture)
{
    if (*texture == 0xFFFF)
    {
        return 0;
    }
    return mTextures[*texture];
}

PlatTexture* glTextureManager::GetTexture(UnidentifiedTextureState* texture)
{
    ResolveTextureIndex(texture);
    unsigned long textureIndex = texture->textureIndex;
    return GetTextureAtIndex(&textureIndex);
}

void glTextureManager::RegisterTexture(PlatTexture* texture)
{
    texture->unknown0E = mFreeIndices->RemoveStart();
    mTextures[texture->unknown0E] = texture;
}

void glTextureManager::RegisterTextureAnim(GLTextureAnim* anim)
{
    unsigned long index = mFreeIndices->RemoveStart();
    anim->m_unk_0x18 = index;
    mTextures[index] = GetTextureAtIndex(&anim->GetTexture(-1)->textureHandle);
}

void glTextureManager::RefreshTextureAnim(GLTextureAnim* anim)
{
    u32 index = anim->m_unk_0x18;
    mTextures[index] = GetTextureAtIndex(&anim->GetTexture(-1)->textureHandle);
}
