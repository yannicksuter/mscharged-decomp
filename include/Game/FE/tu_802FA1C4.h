#ifndef GAME_FE_TU_802FA1C4_H
#define GAME_FE_TU_802FA1C4_H

#include "NL/nlList.h"
#include "types.h"

class TU80300104Base;

class TU802FA1C4
{
public:
    TU802FA1C4();
    ~TU802FA1C4();

    void fn_802FA2FC(TU80300104Base* listener);
    void fn_802FA388(TU80300104Base* listener);

    /* 0x00 */ nlListContainer<TU80300104Base*> mUnidentified000;
    /* 0x0C */ u32 mUnidentified00C[4];
    /* 0x1C */ u32 mUnidentified01C;
    /* 0x20 */ float mUnidentified020[4][2];
    /* 0x40 */ u16 mUnidentified040[4];
    /* 0x48 */ bool mUnidentified048[4];
    /* 0x4C */ bool mUnidentified04C[4];
    /* 0x50 */ int mUnidentified050;
    /* 0x54 */ int mUnidentified054;
}; // size 0x58

extern TU802FA1C4* lbl_806E2030;

#endif // GAME_FE_TU_802FA1C4_H
