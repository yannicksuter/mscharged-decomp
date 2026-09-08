#ifndef GAME_FE_FEBACKBUTTON_H
#define GAME_FE_FEBACKBUTTON_H

#include "Game/FE/feLibObject.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;

class FEBackButton : public FEPointerButton
{
public:
    FEBackButton();
    virtual ~FEBackButton();

    virtual void OnPointerEnter(int index, void* context);
    virtual void OnPointerLeave(int index, void* context);
    virtual void OnPointerInside(int index, void* context);
    virtual void OnPointerPress(int index, void* context);
    virtual void OnPointerRelease(int index, void* context);
    virtual void SetBackScene(int value);
    virtual void SetButtonInstance(TLComponentInstance* instance);
    virtual bool UpdateBackButton(FEPointerEvent event, float fDeltaT);
    virtual void SetPushBackScene(bool value) { mPushBackScene = value; }
    virtual void SetPopScene(bool value) { mPopScene = value; }

    /* 0x0B4 */ int mBackScene;
    /* 0x0B8 */ float mUnidentifiedB8;
    /* 0x0BC */ feVector3 mButtonPosition;
    /* 0x0C8 */ TLComponentInstance* mButtonInstance;
    /* 0x0CC */ bool mPressed;
    /* 0x0CD */ bool mUnidentifiedCD;
    /* 0x0CE */ bool mUnidentifiedCE;
    /* 0x0CF */ bool mPushBackScene;
    /* 0x0D0 */ bool mPopScene;
    /* 0x0D1 */ bool mBoundsInitialized;
    /* 0x0D2 */ bool mPointerInside[4];
}; // size 0xD8

#endif // GAME_FE_FEBACKBUTTON_H
