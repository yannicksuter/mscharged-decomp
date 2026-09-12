#ifndef NL_GLX_CHARACTER_DAMAGE_H
#define NL_GLX_CHARACTER_DAMAGE_H

extern "C" void glxConfigureCharacterDamage(float megaBlend, const bool* damageEnabled, int stageCount, int texGenCount,
    int damageTexCoord1, int damageTexCoord2, int megaTexCoord,
    int damageTexture1, int damageTexture2, int megaTexture);

#endif // NL_GLX_CHARACTER_DAMAGE_H
