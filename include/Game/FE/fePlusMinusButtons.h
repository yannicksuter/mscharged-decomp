#ifndef GAME_FE_PLUS_MINUS_BUTTONS_H
#define GAME_FE_PLUS_MINUS_BUTTONS_H

#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

struct FEPageControls
{
    FEPageControls(bool enabled);
    virtual ~FEPageControls();
    virtual void Update(FEPointerEvent event, float fDeltaT);
    virtual void SetPlusButton(TLComponentInstance* component);
    virtual void SetMinusButton(TLComponentInstance* component);

    void SetButtonState(int index, bool enabled, bool visible);

    /* 0x004 */ bool mUnidentified004;
    /* 0x005 */ bool mUnidentified005[2];
    /* 0x007 */ bool mUnidentified007[2];
    /* 0x009 */ bool mUnidentified009[2];
    /* 0x00B */ bool mUnidentified00B;
    /* 0x00C */ bool mUnidentified00C;
    /* 0x00D */ bool mUnidentified00D;
    /* 0x00E */ bool mUnidentified00E;
    /* 0x00F */ unsigned char mPadding00F;
    /* 0x010 */ int mUnidentified010;
    /* 0x014 */ float mUnidentified014;
    /* 0x018 */ FEPointerButton mRegions[2];
    /* 0x180 */ TLComponentInstance* mPlusButton;
    /* 0x184 */ TLComponentInstance* mMinusButton;
}; // size 0x188

#endif // GAME_FE_PLUS_MINUS_BUTTONS_H
