#ifndef GAME_AI_AIPAD_H
#define GAME_AI_AIPAD_H

#include "types.h"
#include "NL/globalpad.h"

class cAIPad
{
public:
    u16 GetMovementStickDirection();
    float GetMovementStickMagnitude();

private:
    /* 0x000 */ u8 mUnknown000[0x2DC];

public:
    /* 0x2DC */ cGlobalPad* m_pGlobalPad;
}; // total size: 0x2E0

#endif // GAME_AI_AIPAD_H
