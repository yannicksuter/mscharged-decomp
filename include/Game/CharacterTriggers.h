#ifndef GAME_CHARACTERTRIGGERS_H
#define GAME_CHARACTERTRIGGERS_H

class cCharacter;
class cPlayer;

void GetAnimTriggerInfo(cCharacter* pCharacter, int animIndex,
    bool (*callback)(float, float, unsigned long, float, void*), void* pData);
void EmitGoalieCatch(cPlayer* pPlayer, const char* name, bool bRumble);

#endif // GAME_CHARACTERTRIGGERS_H
