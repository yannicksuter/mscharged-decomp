#ifndef GAME_SH_HALL_OF_FAME_ROOM_H
#define GAME_SH_HALL_OF_FAME_ROOM_H

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class FEPageControls;

class SHHallOfFameRoom : public BaseSceneHandler
{
public:
    virtual ~SHHallOfFameRoom();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    virtual void InitializeButtons() = 0;
    virtual void HandlePointerEvent(const FEPointerEvent* event, int index, float fDeltaT) = 0;

    /* 0x01C */ FEBackButton mBackButton;
    /* 0x0F4 */ FEPageControls* mPageControls;
    /* 0x0F8 */ int mMode;
    /* 0x0FC */ int mPointerHoverCounts[4];
    /* 0x10C */ bool mButtonsInitialized;
    /* 0x110 */ float mTrophyTimers[7];
}; // size 0x12C

class SHHallOfFameCup : public SHHallOfFameRoom
{
public:
    SHHallOfFameCup(int mode);
    virtual ~SHHallOfFameCup();
    virtual void SceneCreated();
    virtual void InitializeButtons();
    virtual void HandlePointerEvent(const FEPointerEvent* event, int index, float fDeltaT);

    void OnItemPointerInside(int index, void* context);
    void OnItemPointerEnter(int index, void* context);
    void OnItemPointerLeave(int index, void* context);
    void OnItemPointerPress(int index, void* context);
    void OnProgressPointerEnter(int index, void* context);
    void OnProgressPointerLeave(int index, void* context);
    void OnProgressPointerPress(int index, void* context);
    void ShowLockedItemMessage(unsigned int index);

    /* 0x12C */ FEPointerButton mCupButton;
    /* 0x1E0 */ FEPointerButton mPlayerCardButtons[4];
    /* 0x4B0 */ FEPointerButton mAwardButtons[2];
    /* 0x618 */ FEPointerButton mProgressButton;
    /* 0x6CC */ TLComponentInstance* mProgressButtonInstance;
    /* 0x6D0 */ TLComponentInstance* mCupInstance;
    /* 0x6D4 */ TLComponentInstance* mPlayerCardInstances[4];
    /* 0x6E4 */ TLComponentInstance* mAwardInstances[2];
    /* 0x6EC */ TLComponentInstance* mRollovers;
}; // size 0x6F0

class SHHallOfFameProfile : public SHHallOfFameRoom
{
public:
    SHHallOfFameProfile();
    virtual ~SHHallOfFameProfile();
    virtual void SceneCreated();
    virtual void InitializeButtons();
    virtual void HandlePointerEvent(const FEPointerEvent* event, int index, float fDeltaT);

    void OnSummaryPointerEnter(int index, void* context);
    void OnSummaryPointerLeave(int index, void* context);
    void OnSummaryPointerPress(int index, void* context);

    /* 0x12C */ FEPointerButton mSummaryButton;
    /* 0x1E0 */ TLComponentInstance* mSummaryButtonInstance;
}; // size 0x1E4

#endif // GAME_SH_HALL_OF_FAME_ROOM_H
