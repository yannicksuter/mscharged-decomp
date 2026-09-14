#ifndef GAME_FE_FE_CAPTAIN_COMPONENT_H
#define GAME_FE_FE_CAPTAIN_COMPONENT_H

#include "types.h"
#include "Game/FE/feScrollBar.h"
#include "Game/FE/feScrollText.h"
#include "Game/FE/feTimer.h"

class TLComponentInstance;
class TLImageInstance;
class TLInstance;
struct UnidentifiedTLGroupInstance;

struct TU801DE42CComponent
{
    TU801DE42CComponent(TLComponentInstance* component);
    void SetValue(int value);

    /* 0x00 */ int mUnidentified00;
    /* 0x04 */ TLComponentInstance* mComponent;
}; // size 0x8

class TU801DA134Component
{
public:
    TU801DA134Component();
    virtual ~TU801DA134Component();

    void fn_801DA198(TLComponentInstance* component, int side);
    void fn_801DA718(TLInstance* instance, int captain, unsigned char alpha);
    void fn_801DA88C();
    TLImageInstance* fn_801DA924(int index, const char* name);
    void fn_801DABAC(int index, int state);
    void fn_801DAFC8();
    void fn_801DB69C(float dt);
    void fn_801DCB28();
    void fn_801DCC28();
    void fn_801DCCEC();
    void fn_801DC824(bool visible0, bool visible1, bool visible2);
    int fn_801DCD74(int index);
    void fn_801DCD84(int value);
    void fn_801DCD8C(int index, int value);

    /* 0x04 */ TLComponentInstance* mComponent;
    /* 0x08 */ TLComponentInstance* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
    /* 0x10 */ int mSide;
    /* 0x14 */ int mUnidentified14;
    /* 0x18 */ int mSidekicks[3];
    /* 0x24 */ int mUnidentified24;
}; // size 0x28

class TU801DCD9CComponent
{
public:
    TU801DCD9CComponent();
    virtual ~TU801DCD9CComponent();

    void fn_801DCFD8(TLComponentInstance* component, int side, u32 value);
    void fn_801DE570(bool visible);
    void fn_801E0B20(bool visible);
    void fn_801DE584(float dt);
    void ResetAttributes();
    void fn_801DEB50(FETimer* timer, TU801DE42CComponent* component, int value);
    void fn_801DEDD0(int captain, int index, unsigned long flag);
    void fn_801DF85C(int sidekick, int index, unsigned long flag);
    void fn_801E0280(int value);
    void fn_801E0AD0();
    void fn_801E0B8C(int captain, int opponent);
    void fn_801E0D8C(TLInstance* instance, int captain, unsigned char alpha);
    void fn_801E0F14(bool fire, bool crystal, bool striker);

    /* 0x004 */ TU801DE42CComponent* mUnidentified04[4];
    /* 0x014 */ TU801DE42CComponent* mUnidentified14[4];
    /* 0x024 */ u32 mUnidentified24;
    /* 0x028 */ UnidentifiedTimerList_80306294 mTimers;
    /* 0x034 */ FEScrollText mScrollText;
    /* 0x074 */ TLComponentInstance* mUnidentified74;
    /* 0x078 */ TLComponentInstance* mUnidentified78;
    /* 0x07C */ TLComponentInstance* mUnidentified7C;
    /* 0x080 */ TLComponentInstance* mUnidentified80;
    /* 0x084 */ TLComponentInstance* mUnidentified84;
    /* 0x088 */ TLComponentInstance* mUnidentified88;
    /* 0x08C */ UnidentifiedTLGroupInstance* mUnidentified8C;
    /* 0x090 */ TLComponentInstance* mUnidentified90;
    /* 0x094 */ TLComponentInstance* mUnidentified94;
    /* 0x098 */ TLImageInstance* mUnidentified98[7];
    /* 0x0B4 */ TLImageInstance* mUnidentifiedB4[7];
    /* 0x0D0 */ TLComponentInstance* mUnidentifiedD0;
    /* 0x0D4 */ TLComponentInstance* mUnidentifiedD4;
    /* 0x0D8 */ UnidentifiedTLGroupInstance* mUnidentifiedD8;
    /* 0x0DC */ FEScrollBar mScrollBar;
    /* 0x290 */ int mUnidentified290;
    /* 0x294 */ int mUnidentified294;
    /* 0x298 */ int mUnidentified298;
    /* 0x29C */ int mUnidentified29C;
    /* 0x2A0 */ int mUnidentified2A0;
    /* 0x2A4 */ bool mUnidentified2A4;
    /* 0x2A5 */ bool mUnidentified2A5;
    /* 0x2A6 */ bool mUnidentified2A6;
    /* 0x2A7 */ bool mUnidentified2A7;
    /* 0x2A8 */ bool mUnidentified2A8;
}; // size 0x2AC

#endif // GAME_FE_FE_CAPTAIN_COMPONENT_H
