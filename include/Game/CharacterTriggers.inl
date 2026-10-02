#ifndef GAME_CHARACTER_TRIGGERS_INL
#define GAME_CHARACTER_TRIGGERS_INL

#include "Game/CharacterTriggers.h"
#include "Game/Player.h"

inline bool HasGlobalPad(cPlayer* pPlayer)
{
    return pPlayer->GetGlobalPad() != 0;
}

#endif // GAME_CHARACTER_TRIGGERS_INL
