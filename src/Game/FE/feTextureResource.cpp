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

extern "C" void fn_8030009C(FETextureResource* texture, const unsigned long* handle)
{
    texture->m_glTextureHandle = *handle;
    if (glTextureLoad(*handle))
    {
        texture->m_width = glTextureGetWidth();
        texture->m_height = glTextureGetHeight();
    }
    else
    {
        texture->m_width = 1;
        texture->m_height = 1;
    }
}

unsigned long FETextureResource::GetTextureHandle() const
{
    return m_glTextureHandle;
}
