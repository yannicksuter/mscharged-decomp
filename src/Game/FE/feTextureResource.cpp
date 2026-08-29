#include "Game/FE/feTextureResource.h"

#include "NL/gl/glTexture.h"

FETextureResource::FETextureResource()
{
    m_next = 0;
    m_prev = 0;
    m_bValid = false;
    field_0x14 = 0;
    m_glTextureHandle = 0;
    m_type = FERT_TEXTURE;
}

void FETextureResource::fn_8030009C(const unsigned long& textureReference)
{
    m_glTextureHandle = textureReference;
    if (glTextureLoad(textureReference))
    {
        m_uWidth = glTextureGetWidth();
        m_uHeight = glTextureGetHeight();
    }
    else
    {
        m_uWidth = 1;
        m_uHeight = 1;
    }
}

u32 FETextureResource::GetTextureHandle() const
{
    return m_glTextureHandle;
}
