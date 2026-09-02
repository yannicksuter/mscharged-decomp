#ifndef UNCLASSIFIED_TU_801F8CB0_H
#define UNCLASSIFIED_TU_801F8CB0_H

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/BaseOverlayHandler.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;

class TU801F8CB0Overlay : public BaseOverlayHandler
{
public:
    TU801F8CB0Overlay(ScreenMovement movement);
    virtual ~TU801F8CB0Overlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801F9BE4(int index, void* context);
    void fn_801F9C6C(int index, void* context);
    void fn_801F9CDC(int index, void* context);

    /* 0x028 */ TU80219248Component mControllerComponent;
    /* 0x0DC */ TLComponentInstance* mUnidentified0DC;
    /* 0x0E0 */ ScreenMovement mMovement;
    /* 0x0E4 */ bool mUnidentified0E4;
    /* 0x0E5 */ bool mUnidentified0E5;
    /* 0x0E6 */ u16 mText[4][0x80];
    /* 0x4E6 */ u8 mPadding4E6[2];
    /* 0x4E8 */ int mState;
}; // size 0x4EC

#endif // UNCLASSIFIED_TU_801F8CB0_H
