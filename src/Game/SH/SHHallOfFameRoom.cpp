#include "Game/SH/SHHallOfFameRoom.h"
#include "Game/FE/FEAudio.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/FE/feDPD.h"
#include "NL/nlBind.h"

SHHallOfFameRoom::~SHHallOfFameRoom()
{
}

void StopHallOfFameTrophyEffects()
{
    EmissionManager* manager = EmissionManager::Instance();
    EffectsGroup* group = manager->GetEffectsGroup("fe_light_cone_trophy");
    if (group != 0)
    {
        manager->Kill(group);
    }
    FEAudio::StopAnimAudioEvent(0x1C4C829E, (void*)1);
}

void SHHallOfFameCup::HandlePointerEvent(
    const FEPointerEvent* event, int index, float)
{
    mCupButton.HandlePointerEvent(event);
    mProgressButton.HandlePointerEvent(event);

    for (int i = 0; i < 2; ++i)
    {
        mAwardButtons[i].HandlePointerEvent(event);
    }
    for (int i = 0; i < 4; ++i)
    {
        mPlayerCardButtons[i].HandlePointerEvent(event);
    }

    if (mPointerHoverCounts[index] <= 0)
    {
        mRollovers->m_bVisible = false;
    }
}

void SHHallOfFameCup::InitializeButtons()
{
    FEPointerListener::Callback enterFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnItemPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leaveFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnItemPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback pressFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnItemPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback insideFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnItemPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressEnterFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnProgressPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressLeaveFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnProgressPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressPressFunc(Bind<void>(MemFun(&SHHallOfFameCup::OnProgressPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    mCupButton.SetInstanceBounds(mCupInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mCupButton.SetPointerEnterCallback(enterFunc);
    mCupButton.SetPointerLeaveCallback(leaveFunc);
    mCupButton.SetPointerPressCallback(pressFunc);
    mProgressButton.SetInstanceBounds(mProgressButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mProgressButton.SetPointerEnterCallback(progressEnterFunc);
    mProgressButton.SetPointerLeaveCallback(progressLeaveFunc);
    mProgressButton.SetPointerPressCallback(progressPressFunc);

    for (int i = 0; i < 2; ++i)
    {
        mAwardButtons[i].SetInstanceBounds(mAwardInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mAwardButtons[i].SetPointerEnterCallback(enterFunc);
        mAwardButtons[i].SetPointerLeaveCallback(leaveFunc);
        mAwardButtons[i].SetPointerInsideCallback(insideFunc);
        mAwardButtons[i].SetPointerPressCallback(pressFunc);
    }
    for (int i = 0; i < 4; ++i)
    {
        mPlayerCardButtons[i].SetInstanceBounds(mPlayerCardInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mPlayerCardButtons[i].SetPointerEnterCallback(enterFunc);
        mPlayerCardButtons[i].SetPointerLeaveCallback(leaveFunc);
        mPlayerCardButtons[i].SetPointerInsideCallback(insideFunc);
        mPlayerCardButtons[i].SetPointerPressCallback(pressFunc);
    }
}

void SHHallOfFameCup::OnItemPointerInside(int, void*)
{
}

void SHHallOfFameCup::OnItemPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;

    --mPointerHoverCounts[index];
    mTrophyTimers[item] = 0.1f;

    if (item == 0)
    {
        mCupButton.SetPointerState(0, index);
    }
    else if (item <= 2)
    {
        mAwardButtons[item - 1].SetPointerState(0, index);
    }
    else if (item <= 6)
    {
        mPlayerCardButtons[item - 3].SetPointerState(0, index);
    }
}

void SHHallOfFameCup::OnProgressPointerEnter(int index, void*)
{
    ++mPointerHoverCounts[index];
    if (!mProgressButton.HasOtherPointerState(1, index))
    {
        mProgressButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
        mProgressButton.SetPointerState(1, index);
    }
}

void SHHallOfFameCup::OnProgressPointerLeave(int index, void*)
{
    --mPointerHoverCounts[index];
    if (!mProgressButton.HasOtherPointerState(1, index))
    {
        mProgressButtonInstance->SetActiveSlide("off", true, false);
        mProgressButton.SetPointerState(0, index);
    }
}

void SHHallOfFameCup::OnProgressPointerPress(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    CycleHallOfFameDetailPage(mMode, 0);
}

void SHHallOfFameProfile::HandlePointerEvent(
    const FEPointerEvent* event, int, float)
{
    mSummaryButton.HandlePointerEvent(event);
}

void SHHallOfFameProfile::InitializeButtons()
{
    FEPointerListener::Callback enterFunc(Bind<void>(MemFun(&SHHallOfFameProfile::OnSummaryPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leaveFunc(Bind<void>(MemFun(&SHHallOfFameProfile::OnSummaryPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback pressFunc(Bind<void>(MemFun(&SHHallOfFameProfile::OnSummaryPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    mSummaryButton.SetInstanceBounds(mSummaryButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mSummaryButton.SetPointerEnterCallback(enterFunc);
    mSummaryButton.SetPointerLeaveCallback(leaveFunc);
    mSummaryButton.SetPointerPressCallback(pressFunc);
}

void SHHallOfFameProfile::OnSummaryPointerEnter(int index, void*)
{
    ++mPointerHoverCounts[index];
    if (!mSummaryButton.HasOtherPointerState(1, index))
    {
        mSummaryButtonInstance->SetActiveSlide("over", true, false);
        mSummaryButton.SetInstanceBounds(
            mSummaryButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
        mSummaryButton.SetPointerState(1, index);
    }
}

void SHHallOfFameProfile::OnSummaryPointerLeave(int index, void*)
{
    --mPointerHoverCounts[index];
    if (!mSummaryButton.HasOtherPointerState(1, index))
    {
        mSummaryButtonInstance->SetActiveSlide("off", true, false);
        mSummaryButton.SetInstanceBounds(
            mSummaryButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
        mSummaryButton.SetPointerState(0, index);
    }
}

void SHHallOfFameProfile::OnSummaryPointerPress(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    CycleHallOfFameDetailPage(4, 0);
}

SHHallOfFameCup::~SHHallOfFameCup()
{
}

SHHallOfFameProfile::~SHHallOfFameProfile()
{
}
