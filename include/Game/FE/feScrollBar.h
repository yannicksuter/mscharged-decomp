#ifndef GAME_FE_FESCROLLBAR_H
#define GAME_FE_FESCROLLBAR_H

#include "Game/FE/feLibObject.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLImageInstance;

class FEScrollBar
{
public:
    FEScrollBar();
    ~FEScrollBar();

    void SetIgnoreInputLock(bool ignoreInputLock);
    void Initialize();
    bool IsScrolling(int direction, bool resetRepeatTimer);
    void fn_8022FDDC(int index, void* context);
    void fn_8022FE94(int index, void* context);
    void fn_8022FFA8(int index, void* context);
    void fn_802300A4(int index, void* context);
    void fn_802301B8(int index, void* context);
    void fn_8023038C(const nlVector3& value);
    void ResetScrolling();
    void fn_80230EE0(int index, void* context);
    void Update(FEPointerEvent event, float fDeltaT);
    void SetComponent(TLComponentInstance* component);
    void SetRange(int maxValue);
    void SetValue(int value);

    /* 0x000 */ TLComponentInstance* mComponent;
    /* 0x004 */ TLComponentInstance* mButtonInstances[2];
    /* 0x00C */ TLImageInstance* mThumb;
    /* 0x010 */ bool mScrolling[2];
    /* 0x012 */ unsigned char mUnidentified012[2];
    /* 0x014 */ bool mUnidentified014[2];
    /* 0x016 */ bool mUnidentified016[2];
    /* 0x018 */ bool mInitialized;
    /* 0x019 */ bool mUnidentified019;
    /* 0x01A */ bool mUnidentified01A;
    /* 0x01B */ bool mIgnoreInputLock;
    /* 0x01C */ feVector3 mAssetPosition;
    /* 0x028 */ float mRepeatTimer;
    /* 0x02C */ float mScrollStep;
    /* 0x030 */ float mTopPosition;
    /* 0x034 */ int mCurrentValue;
    /* 0x038 */ int mMaxValue;
    /* 0x03C */ nlVector3 mUnidentified03C;
    /* 0x048 */ float mUnidentified048;
    /* 0x04C */ FEPointerButton mButtons[2];
}; // size 0x1B4

#endif // GAME_FE_FESCROLLBAR_H
