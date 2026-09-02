#ifndef GAME_CHARACTERTRIGGERS_H
#define GAME_CHARACTERTRIGGERS_H

enum eBallShotEffectType
{
    BALL_EFFECT_S2S_SUPER_SHOT = 0,
    BALL_EFFECT_S2S_SHOT = 1,
    BALL_EFFECT_PERFECT_SHOT = 2,
    BALL_EFFECT_PERFECT_PASS = 3,
    BALL_EFFECT_REGULAR_SHOT = 4,
    BALL_EFFECT_ONETIMER_SHOT = 5,
    BALL_EFFECT_CHIP_SHOT = 6,
    NUM_BALL_EFFECTS = 7,
};

class cCharacter;
class cPlayer;
class cFielder;
class EmissionController;
class cSAnim;

void CharacterTriggerHandler(cSAnim* arg0, unsigned int uParam);
void GetAnimTriggerInfo(cCharacter* pCharacter, int animIndex,
    bool (*callback)(float, float, unsigned long, float, void*), void* pData);
void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble);
void EmitMushroom(cFielder* pFielder, bool bParam);
void KillMushroom(cFielder* pFielder);
void EmitStar(cFielder* pFielder, bool bParam);
void KillStar(cFielder* pFielder);
bool KillDaze(cPlayer* player);
EmissionController* EmitGeneric(cCharacter* pCharacter, const char* baseName, const char* characterName);

#endif // GAME_CHARACTERTRIGGERS_H
