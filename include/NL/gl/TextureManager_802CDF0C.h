#ifndef NL_GL_TEXTURE_MANAGER_802CDF0C_H
#define NL_GL_TEXTURE_MANAGER_802CDF0C_H

#include "types.h"

class GLTextureAnim;
class PlatTexture;
struct UnidentifiedTextureState;

struct TextureIndexQueue_802D3B68
{
    TextureIndexQueue_802D3B68(u16* data, unsigned long capacity)
    {
        m_04 = 0;
        m_08 = 0;
        m_0C = capacity;
        m_00 = data;
    }

    void UnidentifiedAddEnd(const u16& value)
    {
        u16* entry;
        if (m_08 >= m_0C)
        {
            entry = 0;
        }
        else
        {
            u32 index = (m_04 + m_08) % m_0C;
            ++m_08;
            entry = m_00 + index;
        }
        *entry = value;
    }

    u16 UnidentifiedRemoveStart()
    {
        u16* entry;
        if (m_08 == 0)
        {
            entry = m_00 + (m_04 & m_0C);
        }
        else
        {
            --m_08;
            entry = m_00 + m_04;
            ++m_04;
            if (m_04 % m_0C == 0)
            {
                m_04 = 0;
            }
        }
        return *entry;
    }

    /* 0x00 */ u16* m_00;
    /* 0x04 */ u32 m_04;
    /* 0x08 */ u32 m_08;
    /* 0x0C */ u32 m_0C;
};

class TextureManager_802CDF0C
{
public:
    explicit TextureManager_802CDF0C(unsigned long count);
    unsigned long fn_802CE1B8(unsigned long texture);
    void fn_802CE218(UnidentifiedTextureState* texture);
    PlatTexture* fn_802CE294(const unsigned long* texture);
    PlatTexture* fn_802CE2B8(UnidentifiedTextureState* texture);
    void fn_802CE360(PlatTexture* texture);
    void fn_802CE3E0(GLTextureAnim* anim);
    void fn_802CE4B8(GLTextureAnim* anim);

    /* 0x00 */ u32 m_00;
    /* 0x04 */ PlatTexture** m_04;
    /* 0x08 */ TextureIndexQueue_802D3B68* m_08;
    /* 0x0C */ u16* m_0C;
}; // size: 0x10

#endif // NL_GL_TEXTURE_MANAGER_802CDF0C_H
