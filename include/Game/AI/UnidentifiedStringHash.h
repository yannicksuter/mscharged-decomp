#ifndef GAME_AI_UNIDENTIFIED_STRING_HASH_H
#define GAME_AI_UNIDENTIFIED_STRING_HASH_H

#include "Game/AI/Desire.h"

struct UnidentifiedStringHash : public UnidentifiedStateTransition
{
    UnidentifiedStringHash(const char* name);
};

#endif // GAME_AI_UNIDENTIFIED_STRING_HASH_H
