#include "Game/CharacterEffects.h"

#include "NL/gl/glState.h"
#include "NL/glx/glxTexture.h"

static EffectsTexturing fxTexturing[] = {
    EffectsTexturing(0xFFFFFFFF, GLB_None, false, false),
    EffectsTexturing(glGetTexture("global/fx_env_ice"), GLB_ScaledAdditive, false, false),
    EffectsTexturing(glGetTexture("effects/fx_yellow_glow"), GLB_None, false, true),
    EffectsTexturing(glGetTexture("effects/fx_electrocution"), GLB_None, true, false),
};

EffectsTexturing* fxGetTexturing(eEffectsTextureType type)
{
    EffectsTexturing* texturing = &fxTexturing[type];
    unsigned long texture = texturing->m_uTexture;
    if (texture != 0xFFFFFFFF
        && texturing->m_ResolvedTexture.value == 0xFFFF)
    {
        texturing->m_ResolvedTexture.value =
            glGetTextureManager()->GetTextureIndex(texture);
    }
    return texturing;
}
