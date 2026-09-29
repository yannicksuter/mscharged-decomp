#ifndef GAME_CHARACTER_QUERIES_H
#define GAME_CHARACTER_QUERIES_H

#include "Game/Character.h"

inline bool IsCharacterFielder(cCharacter* character)
{
    return character->m_eClassType == FIELDER;
}

inline bool IsCharacterHammerBro(cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass == 13;
}

extern "C" inline bool fn_80194674(cCharacter* character)
{
    return character->mUnidentified024.m_eCharacterClass == 10;
}

#endif // GAME_CHARACTER_QUERIES_H
