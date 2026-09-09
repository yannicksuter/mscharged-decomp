#ifndef GAME_FE_FE_CAPTAIN_COMPONENT_H
#define GAME_FE_FE_CAPTAIN_COMPONENT_H

#include "types.h"

class BaseGameSceneManager;
class TLComponentInstance;

class TU801DA134Component
{
public:
    TU801DA134Component();
    virtual ~TU801DA134Component();

    void fn_801DA198(TLComponentInstance* component, int side);
    void fn_801DA88C();
    void fn_801DCB28();
    void fn_801DCC28();
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

    void fn_801E0B20(bool visible);
    void fn_801DEDD0(int captain, int index, unsigned long flag);
    void fn_801E0280(int value);

    /* 0x004 */ u8 mUnidentified04[0x2A8];
}; // size 0x2AC

void fn_801E23A4(BaseGameSceneManager* manager, u32 from, u32 to);

#endif // GAME_FE_FE_CAPTAIN_COMPONENT_H
