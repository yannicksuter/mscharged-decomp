#ifndef GAME_CHARACTER_EFFECTS_H
#define GAME_CHARACTER_EFFECTS_H

#include "types.h"

enum eEffectsTextureType
{
    eFXTex_Nothing = 0,
    eFXTex_Freeze = 1,
    eFXTex_Star = 2,
    eFXTex_Electrocution = 3,
    eFXTex_Num = 4,
};

enum eGLBlend
{
    GLB_None = 0,
    GLB_Standard = 1,
    GLB_Additive = 2,
    GLB_ScaledAdditive = 3,
    GLB_Luminance = 4,
    GLB_Screen = 5,
    GLB_PassThru = 6,
    GLB_Subtractive = 7,
    GLB_Num = 8,
};

struct ResolvedTexture
{
    u32 value;
};

class EffectsTexturing
{
public:
    EffectsTexturing(unsigned long texture, eGLBlend blendMode, bool enviro, bool detail)
    {
        m_uTexture = texture;
        m_eBlendMode = blendMode;
        m_bEnviro = enviro;
        m_bDetail = detail;
        m_ResolvedTexture.value = 0xFFFF;
    }

    /* 0x00 */ unsigned long m_uTexture;
    /* 0x04 */ ResolvedTexture m_ResolvedTexture;
    /* 0x08 */ eGLBlend m_eBlendMode;
    /* 0x0C */ bool m_bEnviro;
    /* 0x0D */ bool m_bDetail;
}; // total size: 0x10

EffectsTexturing* fxGetTexturing(eEffectsTextureType type);

#endif // GAME_CHARACTER_EFFECTS_H
