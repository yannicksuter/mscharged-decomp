#include "NL/gl/TextureManager_802CDF0C.h"

#include "Game/GL/GLTextureAnim.h"
#include "NL/gl/glModel.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlMemory.h"

#include <string.h>

TextureManager_802CDF0C::TextureManager_802CDF0C(unsigned long count)
{
    m_00 = count;
    unsigned long size = count * sizeof(PlatTexture*);
    m_04 = (PlatTexture**)nlMalloc(size, 8, false);
    memset(m_04, 0, size);
    m_0C = new (8, false) u16[count];
    m_08 = new (8, false) TextureIndexQueue_802D3B68(m_0C, count);
    for (u32 i = 0; i < count; ++i)
    {
        m_08->UnidentifiedAddEnd((u16)i);
    }
}

unsigned long TextureManager_802CDF0C::fn_802CE1B8(unsigned long texture)
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

void TextureManager_802CDF0C::fn_802CE218(UnidentifiedTextureState* texture)
{
    if (texture->textureIndex == 0xFFFF)
    {
        texture->textureIndex = fn_802CE1B8(texture->texture);
    }
}

PlatTexture* TextureManager_802CDF0C::fn_802CE294(const unsigned long* texture)
{
    if (*texture == 0xFFFF)
    {
        return 0;
    }
    return m_04[*texture];
}

PlatTexture* TextureManager_802CDF0C::fn_802CE2B8(UnidentifiedTextureState* texture)
{
    fn_802CE218(texture);
    unsigned long textureIndex = texture->textureIndex;
    return fn_802CE294(&textureIndex);
}

void TextureManager_802CDF0C::fn_802CE360(PlatTexture* texture)
{
    texture->unknown0E = m_08->UnidentifiedRemoveStart();
    m_04[texture->unknown0E] = texture;
}

void TextureManager_802CDF0C::fn_802CE3E0(GLTextureAnim* anim)
{
    unsigned long index = m_08->UnidentifiedRemoveStart();
    anim->m_unk_0x18 = index;
    m_04[index] = fn_802CE294(&anim->GetTexture(-1)->textureHandle);
}

void TextureManager_802CDF0C::fn_802CE4B8(GLTextureAnim* anim)
{
    u32 index = anim->m_unk_0x18;
    m_04[index] = fn_802CE294(&anim->GetTexture(-1)->textureHandle);
}
