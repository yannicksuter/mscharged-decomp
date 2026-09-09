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
    GLTextureAnim* anim = glGetTextureAnim(texture);
    if (anim != 0)
    {
        return anim->m_textureIndex;
    }
    PlatTexture* platformTexture = glx_GetTex(texture);
    if (platformTexture == 0)
    {
        return 0xFFFF;
    }
    return platformTexture->m_TextureIndex;
}

void glTextureManager::ResolveTextureIndex(glTextureBinding* texture)
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

PlatTexture* glTextureManager::GetTexture(glTextureBinding* texture)
{
    ResolveTextureIndex(texture);
    unsigned long textureIndex = texture->textureIndex;
    return GetTextureAtIndex(&textureIndex);
}

void glTextureManager::RegisterTexture(PlatTexture* texture)
{
    texture->m_TextureIndex = mFreeIndices->RemoveStart();
    mTextures[texture->m_TextureIndex] = texture;
}

void glTextureManager::RegisterTextureAnim(GLTextureAnim* anim)
{
    unsigned long index = mFreeIndices->RemoveStart();
    anim->m_textureIndex = index;
    mTextures[index] = GetTextureAtIndex(&anim->GetTexture(-1)->textureHandle);
}

void glTextureManager::RefreshTextureAnim(GLTextureAnim* anim)
{
    u32 index = anim->m_textureIndex;
    mTextures[index] = GetTextureAtIndex(&anim->GetTexture(-1)->textureHandle);
}
