#ifndef GAME_SH_SHCHALLENGESELECT_H
#define GAME_SH_SHCHALLENGESELECT_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feScrollBar.h"

class FEPresentation;
class TLComponentInstance;

class ChallengeSelectScene : public BaseSceneHandler
{
public:
    ChallengeSelectScene(bool tutorial);
    virtual ~ChallengeSelectScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void OnChallengePressed(int pointerIndex, void* context);
    void OnChallengeEnter(int pointerIndex, void* context);
    void OnChallengeLeave(int pointerIndex, void* context);
    void OnChallengeInside(int pointerIndex, void* context);
    bool IsChallengeAvailable(int challenge);
    void UpdateRow(int row);
    void InitializeButtons();
    void UpdateRows()
    {
        for (int row = 0; row < 3; ++row)
        {
            UpdateRow(row);
        }
    }

    /* 0x01C */ int mChallengeOffset;
    /* 0x020 */ int mMaxScrollOffset;
    /* 0x024 */ int mPointerInsideCount;
    /* 0x028 */ bool mButtonsInitialized;
    /* 0x029 */ bool mTutorial;
    /* 0x02A */ unsigned char mPadding02A[2];
    /* 0x02C */ FEBackButton mBackButton;
    /* 0x104 */ FEPointerButton mChallengeButtons[3];
    /* 0x320 */ FEScrollBar mScrollBar;
    /* 0x4D4 */ TLComponentInstance* mChallengeSlides[3];
    /* 0x4E0 */ unsigned short mChallengeNumbers[3][8];
    /* 0x510 */ unsigned short mChallengeCountText[8];
    /* 0x520 */ int mState;
}; // size 0x524

#endif // GAME_SH_SHCHALLENGESELECT_H
