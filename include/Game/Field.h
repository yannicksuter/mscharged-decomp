#ifndef GAME_FIELD_H
#define GAME_FIELD_H

#include "Game/Net.h"

class cField
{
public:
    static cNet* GetNet(float side);
    static float GetGoalLineX(unsigned int side);
    static float GetPenaltyBoxX(unsigned int side);
};

#endif // GAME_FIELD_H
