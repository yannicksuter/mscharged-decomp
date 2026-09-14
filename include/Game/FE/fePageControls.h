#ifndef GAME_FE_FEPAGECONTROLS_H
#define GAME_FE_FEPAGECONTROLS_H

#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

struct FEPageControls
{
    FEPageControls(bool enabled);
    virtual ~FEPageControls();
    virtual void Update(FEPointerEvent event, float fDeltaT);
    virtual void SetPlusButton(TLComponentInstance* component);
    virtual void SetMinusButton(TLComponentInstance* component);
    virtual void OnPointerEnter(int index, void* context);
    virtual void OnPointerLeave(int index, void* context);
    virtual void OnPointerPress(int index, void* context);
    virtual void OnPadPress(int index, void* context);

    bool IsButtonPressed(int direction) const
    {
        return mPointerPressed[direction] || mPadPressed[direction];
    }

    void SetButtonState(int direction, bool enabled, bool visible);
    void InitializeButtons();
    void ClearButtonHighlight(int index);

    /* 0x004 */ bool mInitialized;
    /* 0x005 */ bool mPointerPressed[2];
    /* 0x007 */ bool mPointerInside[2];
    /* 0x009 */ bool mPadPressed[2];
    /* 0x00B */ bool mPadInputEnabled;
    /* 0x00C */ bool mPointerHeld;
    /* 0x00D */ bool mPadHeld[2];
    /* 0x00F */ unsigned char mPadding00F;
    /* 0x010 */ int mHeldButton;
    /* 0x014 */ float mRepeatTime;
    /* 0x018 */ FEPointerButton mButtons[2];
    /* 0x180 */ TLComponentInstance* mButtonInstances[2];
}; // size 0x188

#endif // GAME_FE_FEPAGECONTROLS_H
