#ifndef UNCLASSIFIED_TU_80209584_H
#define UNCLASSIFIED_TU_80209584_H

#include "types.h"
#include "Game/DB/StatsTracker.h"

class FEPresentation;
class TLInstance;

class TU80209584Summary
{
public:
    TU80209584Summary();
    ~TU80209584Summary();

    void fn_802095D0(TeamStats home, TeamStats away, FEPresentation* presentation);
    void fn_8020A014(int side, int row, int value, int extra, TLInstance* instance);

    /* 0x000 */ PlayerStats mStatsA;
    /* 0x054 */ PlayerStats mStatsB;
    /* 0x0A8 */ FEPresentation* mPresentation;
    /* 0x0AC */ int mTeamA;
    /* 0x0B0 */ int mTeamB;
    /* 0x0B4 */ u16 mBuffersColBySide[2][7][0x20];
}; // size 0x434

#endif // UNCLASSIFIED_TU_80209584_H
