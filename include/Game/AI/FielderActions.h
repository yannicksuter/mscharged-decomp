#ifndef GAME_AI_FIELDERACTIONS_H
#define GAME_AI_FIELDERACTIONS_H

#include "types.h"

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

#endif // GAME_AI_FIELDERACTIONS_H
