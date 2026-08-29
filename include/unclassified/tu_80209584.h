#ifndef UNCLASSIFIED_TU_80209584_H
#define UNCLASSIFIED_TU_80209584_H

#include "types.h"

class FEPresentation;
class TLInstance;

struct TU80209584Stats
{
    /* 0x00 */ u16 mValue00;
    /* 0x02 */ u16 mValue02;
    /* 0x04 */ u16 mValue04;
    /* 0x06 */ u16 mValue06;
    /* 0x08 */ u16 mValue08;
    /* 0x0A */ u16 mValue0A;
    /* 0x0C */ u16 mValue0C;
    /* 0x0E */ u16 mValue0E;
    /* 0x10 */ u16 mValue10;
    /* 0x12 */ u16 mValue12;
    /* 0x14 */ u16 mValue14;
    /* 0x16 */ u16 mValue16;
    /* 0x18 */ u16 mValue18;
    /* 0x1A */ u16 mValue1A;
    /* 0x1C */ u16 mValue1C;
    /* 0x1E */ u16 mValue1E;
    /* 0x20 */ u16 mValue20;
    /* 0x22 */ u16 mValue22;
    /* 0x24 */ u16 mValue24;
    /* 0x26 */ u16 mValue26;
    /* 0x28 */ u16 mValue28;
    /* 0x2A */ u16 mValue2A;
    /* 0x2C */ u16 mValue2C;
    /* 0x2E */ u16 mValue2E;
    /* 0x30 */ u16 mValue30;
    /* 0x32 */ u16 mValue32;
    /* 0x34 */ u16 mValue34;
    /* 0x36 */ u16 mValue36;
    /* 0x38 */ u16 mValue38;
    /* 0x3C */ u32 mValue3C;
    /* 0x40 */ u32 mValue40;
    /* 0x44 */ u16 mValue44;
    /* 0x46 */ u16 mValue46;
    /* 0x48 */ u16 mValue48;
    /* 0x4A */ u16 mValue4A;
    /* 0x4C */ u32 mValue4C;
    /* 0x50 */ u32 mValue50;
}; // size 0x54

struct TU80209584StatsSource
{
    /* 0x00 */ int mTeam;
    /* 0x04 */ u8 mUnknown04[0x18];
    /* 0x1C */ TU80209584Stats mStats;
}; // size 0x70

class TU80209584Summary
{
public:
    TU80209584Summary();
    ~TU80209584Summary();

    void fn_802095D0(const TU80209584StatsSource* home, const TU80209584StatsSource* away, FEPresentation* presentation);
    void fn_8020A014(int side, int row, int value, int extra, TLInstance* instance);

    /* 0x000 */ TU80209584Stats mStatsA;
    /* 0x054 */ TU80209584Stats mStatsB;
    /* 0x0A8 */ FEPresentation* mPresentation;
    /* 0x0AC */ int mTeamA;
    /* 0x0B0 */ int mTeamB;
    /* 0x0B4 */ u16 mBuffersColBySide[2][7][0x20];
}; // size 0x434

#endif // UNCLASSIFIED_TU_80209584_H
