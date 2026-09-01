#ifndef GAME_CHARACTERTRIGGERS_H
#define GAME_CHARACTERTRIGGERS_H

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
