#include "Game/SH/SHHallOfFameRoom.h"
#include "NL/nlFunction.inl"
#include "Game/FE/FEAudio.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/FE/feDPD.h"
#include "NL/nlBind.h"
#include "Game/FE/FEAudio.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/BasicStadium.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePageControls.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

static const char* sRolloverSlides[21] = {
    "FIRE CUP",
    "FIRE CUP BRICK WALL",
    "FIRE CUP GOLDEN BOOT",
    "MARIO",
    "LUIGI",
    "DONKEYKONG",
    "PEACH",
    "CRYSTAL CUP",
    "CRYSTAL CUP BRICK WALL",
    "CRYSTAL CUP GOLDEN BOOT",
    "DAISY",
    "WARIO",
    "WALUIGI",
    "YOSHI",
    "STRIKER CUP",
    "STRIKER CUP BRICK WALL",
    "STRIKER CUP GOLDEN BOOT",
    "BOWSER",
    "PETEY",
    "BOWSERJR",
    "DIDDYKONG",
};

SHHallOfFameRoom::~SHHallOfFameRoom()
{
}

void SHHallOfFameRoom::SceneCreated()
{
    if (IsWidescreen())
    {
        mPresentation->SetActiveSlide("Slide1", true);
    }
    else
    {
        mPresentation->SetActiveSlide("4:3", true);
    }

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    TLComponentInstance* breadcrumbs = 0;
    if (navigation != 0)
    {
        navigation->SetButtons(0x8F, false);
        backButton = navigation->GetButton(4);
        breadcrumbs = navigation->GetButton(8);
        mPageControls = navigation->GetPageControls();
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, true, true);
    }

    mBackButton.SetButtonInstance(backButton);
    SetHallOfFameBreadcrumbs(mMode, breadcrumbs);

    for (int pad = 0; pad < 4; ++pad)
    {
        GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
    }

    FEMusic::StartStreamIfDifferent(12);
}

void SHHallOfFameRoom::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mButtonsInitialized)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }

        InitializeButtons();
        mButtonsInitialized = true;
    }

    for (int i = 0; i < 7; ++i)
    {
        if (mTrophyTimers[i] > 0.0f)
        {
            mTrophyTimers[i] -= fDeltaT;
            if (mTrophyTimers[i] <= 0.0f)
            {
                fn_8027890C(BasicStadium::GetCurrentStadium(), "fe_light_cone_trophy", GetHallOfFameTrophyID(mMode, i));
                mTrophyTimers[i] = 0.0f;
            }
        }
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
                goto checkInput;
            }

            if (mPointerHoverCounts[pad] > 0 || mBackButton.mPointerInside[pad]
                || mPageControls->mPointerInside[0] || mPageControls->mPointerInside[1])
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
        processInput = true;

    checkInput:
        if (processInput)
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            mPageControls->Update(event, fDeltaT);
            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                StopHallOfFameTrophyEffects();
                LeaveHallOfFamePage(mMode);
                return;
            }

            if (mPageControls->IsButtonPressed(1))
            {
                if (g_pFEInput->m_InputLockDepth == 0)
                {
                    StopHallOfFameTrophyEffects();
                    CycleHallOfFameCup(mMode, false);
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                    FEAudio::PlayAnimAudioEvent(0xD982FCA9, 0, 0, 1);
                }
            }
            else if (mPageControls->IsButtonPressed(0))
            {
                if (g_pFEInput->m_InputLockDepth == 0)
                {
                    StopHallOfFameTrophyEffects();
                    CycleHallOfFameCup(mMode, true);
                    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                    FEAudio::PlayAnimAudioEvent(0xD982FCA9, 0, 0, 1);
                }
            }
            else
            {
                HandlePointerEvent(&event, pad, fDeltaT);
            }

            if (mPointerHoverCounts[pad] <= 0)
            {
                FEAudio::StopAnimAudioEvent(0x1C4C829E, (void*)1);
            }
        }
    }
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

inline SHHallOfFameRoom::SHHallOfFameRoom()
{
    mMode = 4;
    mButtonsInitialized = false;
    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;
    StopHallOfFameTrophyEffects();
}

SHHallOfFameCup::SHHallOfFameCup(int mode)
{
    unsigned int nextItem = 0;
    mMode = mode;
    mTrophyTimers[nextItem] = 0.0f;
    mCupButton.mContext = (void*)nextItem++;
    for (int i = 0; i < 2; ++i)
    {
        unsigned int item = nextItem++;
        mTrophyTimers[item] = 0.0f;
        mAwardButtons[i].mContext = (void*)item;
    }
    for (int i = 0; i < 4; ++i)
    {
        unsigned int item = nextItem++;
        mTrophyTimers[item] = 0.0f;
        mPlayerCardButtons[i].mContext = (void*)item;
    }
    mProgressButton.mContext = (void*)nextItem;
}

void SHHallOfFameCup::SceneCreated()
{
    SHHallOfFameRoom::SceneCreated();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mCupInstance = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
        presentation->m_currentSlide, "Layer", "cup");

    char name[8];
    for (int i = 0; i < 2; ++i)
    {
        nlSNPrintf(name, 8, "award_%d", i);
        mAwardInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
            presentation->m_currentSlide, "Layer", name);
    }
    for (int i = 0; i < 4; ++i)
    {
        nlSNPrintf(name, 8, "card_%d", i);
        mPlayerCardInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
            presentation->m_currentSlide, "Layer", name);
    }

    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        mProgressButtonInstance = navigation->GetButton(0x80);
    }

    mRollovers = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
        presentation->m_currentSlide, "Layer", "rollovers");
    mRollovers->m_bVisible = false;
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
    typedef Detail::MemFunImpl<void, void (SHHallOfFameCup::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHHallOfFameCup*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enterFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnItemPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leaveFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnItemPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback pressFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnItemPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback insideFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnItemPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressEnterFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnProgressPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressLeaveFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnProgressPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback progressPressFunc(PointerBinding(MemFun(&SHHallOfFameCup::OnProgressPointerPress), this, Placeholder<0>(), Placeholder<1>()));

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

void SHHallOfFameCup::OnItemPointerInside(unsigned int, void*)
{
}

void SHHallOfFameCup::OnItemPointerEnter(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;

    ++mPointerHoverCounts[index];
    if (mTrophyTimers[item] == 0.0f)
    {
        BasicStadium::GetCurrentStadium()->TriggerEffects(GetHallOfFameTrophyID(mMode, item));
        FEAudio::StopAnimAudioEvent(0x1C4C829E, (void*)1);
        FEAudio::PlayAnimAudioEvent(0x1C4C829E, "FE_HUM", (void*)1, false);
    }
    else
    {
        mTrophyTimers[item] = 0.0f;
        FEAudio::StopAnimAudioEvent(0x1C4C829E, (void*)1);
        FEAudio::PlayAnimAudioEvent(0x1C4C829E, "FE_HUM", (void*)1, false);
    }

    int first = 0;
    switch (mMode)
    {
    case 0:
        first = 0;
        break;
    case 1:
        first = 14;
        break;
    case 2:
        first = 7;
        break;
    }

    mRollovers->m_bVisible = true;
    mRollovers->SetActiveSlide(sRolloverSlides[item + first], true, false);

    if (item == 0)
    {
        mCupButton.SetPointerState(1, index);
    }
    else if (item <= 2)
    {
        mAwardButtons[item - 1].SetPointerState(1, index);
    }
    else if (item <= 6)
    {
        mPlayerCardButtons[item - 3].SetPointerState(1, index);
    }
}

void SHHallOfFameCup::OnItemPointerLeave(unsigned int index, void* context)
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

void SHHallOfFameCup::OnItemPointerPress(unsigned int index, void* context)
{
    int i;
    unsigned int item = (unsigned int)context;

    mPlayerCardButtons[item].SetPointerState(2, index);
    StopHallOfFameTrophyEffects();
    for (i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

    if (!IsUnlockFlagSet(GetHallOfFameUnlockFlag(mMode, item)) && item <= 2)
    {
        ShowLockedItemMessage(item);
    }
    else if (item == 0)
    {
        switch (mMode)
        {
        case 0:
            ShowHallOfFameTrophy(4);
            break;
        case 1:
            ShowHallOfFameTrophy(6);
            break;
        case 2:
            ShowHallOfFameTrophy(5);
            break;
        }
    }
    else if (item <= 2)
    {
        int camera;
        switch (mMode)
        {
        case 0:
            camera = 8;
            if (item - 1 == 0)
            {
                camera = 7;
            }
            ShowHallOfFameTrophy(camera);
            break;
        case 1:
            camera = 12;
            if (item - 1 == 0)
            {
                camera = 11;
            }
            ShowHallOfFameTrophy(camera);
            break;
        case 2:
            camera = 10;
            if (item - 1 == 0)
            {
                camera = 9;
            }
            ShowHallOfFameTrophy(camera);
            break;
        }
    }
    else if (item <= 6)
    {
        switch (mMode)
        {
        case 0:
            ShowHallOfFamePlayerCard(item - 3);
            break;
        case 1:
            ShowHallOfFamePlayerCard(item + 5);
            break;
        case 2:
            ShowHallOfFamePlayerCard(item + 1);
            break;
        }
    }
}

void SHHallOfFameCup::OnProgressPointerEnter(unsigned int index, void*)
{
    ++mPointerHoverCounts[index];
    if (!mProgressButton.HasOtherPointerState(1, index))
    {
        mProgressButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
        mProgressButton.SetPointerState(1, index);
    }
}

void SHHallOfFameCup::OnProgressPointerLeave(unsigned int index, void*)
{
    --mPointerHoverCounts[index];
    if (!mProgressButton.HasOtherPointerState(1, index))
    {
        mProgressButtonInstance->SetActiveSlide("off", true, false);
        mProgressButton.SetPointerState(0, index);
    }
}

void SHHallOfFameCup::OnProgressPointerPress(unsigned int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    CycleHallOfFameDetailPage(mMode, 0);
}

void SHHallOfFameCup::ShowLockedItemMessage(unsigned int item)
{
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        (SceneList)0xA, SCREEN_NOTHING, false);

    ePopupMenu type = INVALID_TYPE;
    if (item == 0)
    {
        switch (mMode)
        {
        case 0:
            type = (ePopupMenu)0x7D;
            break;
        case 1:
            type = (ePopupMenu)0x7E;
            break;
        case 2:
            type = (ePopupMenu)0x7F;
            break;
        }
    }
    else if (item <= 2)
    {
        switch (mMode)
        {
        case 0:
            type = item == 1 ? (ePopupMenu)0x80 : (ePopupMenu)0x81;
            break;
        case 1:
            type = item == 1 ? (ePopupMenu)0x82 : (ePopupMenu)0x83;
            break;
        case 2:
            type = item == 1 ? (ePopupMenu)0x84 : (ePopupMenu)0x85;
            break;
        }
    }

    popup->Create(type, FEPopupMenu::Nothing);
}

SHHallOfFameProfile::SHHallOfFameProfile()
{
    mMode = 0;
    mSummaryButton.mContext = (void*)0;
}

void SHHallOfFameProfile::SceneCreated()
{
    SHHallOfFameRoom::SceneCreated();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mSummaryButtonInstance = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
        presentation->m_currentSlide, "Layer", "summary");
}

void SHHallOfFameProfile::HandlePointerEvent(
    const FEPointerEvent* event, int, float)
{
    mSummaryButton.HandlePointerEvent(event);
}

void SHHallOfFameProfile::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHHallOfFameProfile::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHHallOfFameProfile*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enterFunc(PointerBinding(MemFun(&SHHallOfFameProfile::OnSummaryPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leaveFunc(PointerBinding(MemFun(&SHHallOfFameProfile::OnSummaryPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback pressFunc(PointerBinding(MemFun(&SHHallOfFameProfile::OnSummaryPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    mSummaryButton.SetInstanceBounds(mSummaryButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mSummaryButton.SetPointerEnterCallback(enterFunc);
    mSummaryButton.SetPointerLeaveCallback(leaveFunc);
    mSummaryButton.SetPointerPressCallback(pressFunc);
}

void SHHallOfFameProfile::OnSummaryPointerEnter(unsigned int index, void*)
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

void SHHallOfFameProfile::OnSummaryPointerLeave(unsigned int index, void*)
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

void SHHallOfFameProfile::OnSummaryPointerPress(unsigned int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    CycleHallOfFameDetailPage(4, 0);
}
