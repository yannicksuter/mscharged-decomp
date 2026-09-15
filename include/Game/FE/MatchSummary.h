#ifndef GAME_FE_MATCHSUMMARY_H
#define GAME_FE_MATCHSUMMARY_H

#include "Game/DB/StatsTracker.h"

class FEPresentation;
class TLTextInstance;

class MatchSummary
{
public:
    MatchSummary();
    ~MatchSummary();

    void DisplayMatchSummary(TeamStats home, TeamStats away, FEPresentation* presentation);

private:
    void SetStatValue(int side, int row, int value, int extra, TLTextInstance* instance);
    static const char* GetCaptainTextureName(int team);

    /* 0x000 */ PlayerStats mPlayerStats[2];
    /* 0x0A8 */ FEPresentation* mPresentation;
    /* 0x0AC */ int mTeamIDs[2];
    /* 0x0B4 */ u16 mBuffersColBySide[2][7][0x20];
}; // size 0x434

#endif // GAME_FE_MATCHSUMMARY_H
