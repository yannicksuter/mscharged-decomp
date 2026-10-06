#ifndef GAME_AI_FIELDERACTIONS_H
#define GAME_AI_FIELDERACTIONS_H

#include "types.h"

class AvoidablePolygon;
class cFielder;
class cPlayer;

struct PlayerAttackData
{
    /* 0x00 */ const cPlayer* pAttacker;
    /* 0x04 */ int nAttackerPadID;
    /* 0x08 */ cFielder* pTarget;
    /* 0x0C */ int mUnidentified0C;
    /* 0x10 */ bool mUnidentified10;
    /* 0x11 */ u8 mUnidentified11[3];
}; // total size: 0x14

template <typename T>
class SlotPool;
extern SlotPool<PlayerAttackData> g_PlayerAttackDataPool;
extern AvoidablePolygon* lbl_806E0C74;

void UnFreezeEveryoneButCaptain(cFielder* pCaptain);


float GetMegaStrikeShotCount(cFielder* pFielder, int nParam);
float GetMegaStrikeAccuracy(cFielder* pFielder, int nParam);

#endif // GAME_AI_FIELDERACTIONS_H
