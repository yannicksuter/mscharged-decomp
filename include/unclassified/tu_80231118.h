#ifndef UNCLASSIFIED_TU_80231118_H
#define UNCLASSIFIED_TU_80231118_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feScrollBar.h"

class FEPresentation;
class TLComponentInstance;

class TU80231118Scene : public BaseSceneHandler
{
public:
    TU80231118Scene(bool tutorial);
    virtual ~TU80231118Scene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void fn_80231198(int index, void* context);
    void fn_80231224(int index, void* context);
    void fn_802312CC(int index, void* context);
    void fn_8023134C(int index, void* context);
    bool fn_80231404(int challenge);
    void fn_80231958(int challenge);
    void fn_80232E08();

    /* 0x01C */ int mChallengeOffset;
    /* 0x020 */ int mChallengeCount;
    /* 0x024 */ int mActiveCount;
    /* 0x028 */ bool mPrepared;
    /* 0x029 */ bool mTutorial;
    /* 0x02A */ unsigned char mPadding02A[2];
    /* 0x02C */ FEBackButton mNavigation;
    /* 0x104 */ FEPointerButton mChallengeComponents[3];
    /* 0x320 */ FEScrollBar mScrollWidget;
    /* 0x4D4 */ TLComponentInstance* mChallengeSlides[3];
    /* 0x4E0 */ unsigned short mChallengeNumbers[3][8];
    /* 0x510 */ unsigned short mLayerName[8];
    /* 0x520 */ int mState;
}; // size 0x524

#endif // UNCLASSIFIED_TU_80231118_H
