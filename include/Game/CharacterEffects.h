#ifndef GAME_CHARACTER_EFFECTS_H
#define GAME_CHARACTER_EFFECTS_H

enum eEffectsTextureType
{
    eFXTex_Nothing = 0,
    eFXTex_Electrocution = 3,
};

class EffectsTexturing;

EffectsTexturing* fxGetTexturing(eEffectsTextureType type);

#endif // GAME_CHARACTER_EFFECTS_H
