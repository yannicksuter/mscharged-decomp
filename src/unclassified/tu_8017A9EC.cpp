#include "Game/CharacterEffects.h"

#include "NL/gl/glState.h"

extern "C" void* fn_802CDF0C();
extern "C" ResolvedTexture fn_802CE1B8(void* textureManager, u32 texture);

static EffectsTexturing fxTexturing[] = {
    EffectsTexturing(0xFFFFFFFF, GLB_None, false, false),
    EffectsTexturing(glGetTexture("global/fx_env_ice"), GLB_ScaledAdditive, false, false),
    EffectsTexturing(glGetTexture("effects/fx_yellow_glow"), GLB_None, false, true),
    EffectsTexturing(glGetTexture("effects/fx_electrocution"), GLB_None, true, false),
};

EffectsTexturing* fxGetTexturing(eEffectsTextureType type)
{
    unsigned long texture;
    EffectsTexturing* texturing = &fxTexturing[type];
    texture = texturing->m_uTexture;
    if (texture != 0xFFFFFFFF
        && texturing->m_ResolvedTexture.value == 0xFFFF)
    {
        texturing->m_ResolvedTexture = fn_802CE1B8(fn_802CDF0C(), texture);
    }
    return texturing;
}
