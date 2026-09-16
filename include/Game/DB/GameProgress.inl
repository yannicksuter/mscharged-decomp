#ifndef GAME_DB_GAMEPROGRESS_INL
#define GAME_DB_GAMEPROGRESS_INL

#include "Game/DB/GameProgress.h"

inline StrikerChallenge* fn_801CA670()
{
    return g_pStrikerChallenge;
}

inline int StrikerChallenge::GetCurrentChallenge() const
{
    return mCurrentChallenge;
}

inline int StrikerChallenge::GetRemainingTime() const
{
    return mRemainingTime;
}

inline int StrikerChallenge::GetScore(int side) const
{
    return mScore[side];
}

#endif // GAME_DB_GAMEPROGRESS_INL
