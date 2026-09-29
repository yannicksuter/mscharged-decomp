#include "Game/SH/SHChooseSidekicks.h"
#include "Game/BaseSceneHandler.inl"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinderFind_impl.h"
#include "Game/FE/feFinderDefault_impl.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/fePointer.inl"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHCupNews.h"
#include "NL/nlPrint.h"
#include "NL/nlMemory.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feMusic.h"
#include "Game/Render/FrontEndPresentation.h"
#include "NL/nlString.h"

static int lbl_8051D198[8] = { 1, 0, 5, 4, 3, 2, 6, 7 };
static const nlVector2 lbl_804E8530[2] = { { -159.0f, 187.0f }, { 151.0f, 187.0f } };

static void fn_8022E0C0();
static void fn_8022E18C();

/**
 * Offset/Address/Size: 0x0 | 0x802284B8 | size: 0x3F0
 */
ChooseSidekicksSceneV2::ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement)
    : mPopupActive(false)
    , mMovement(movement)
    , mSceneType(sceneType)
    , mPointerButtonsInitialized(false)
    , mSidekickButtonsInitialized(false)
    , mSidekicksShown(false)
    , mSelectionMade(false)
    , mDoneButtonInstance(0)
    , mDraftExitDone(false)
    , mState(0)
    , mImagesLoaded(false)
{
    int i;

    mSidePads[0] = -1;
    mSidePads[1] = -1;

    for (i = 0; i < 8; ++i)
    {
        mSidekickButtons[i].mContext = (void*)i;
        mSidekickButtons[i].mSpeakerEnabled = false;
    }

    for (int side = 0; side < 2; ++side)
    {
        for (i = 0; i < 3; ++i)
        {
            mSlotButtons[side][i].mContext = (void*)(side * 3 + i);
            mSlotButtons[side][i].mSpeakerEnabled = false;
            mSlotClickButtons[side][i].mContext = (void*)(side * 3 + i);
            mSlotClickButtons[side][i].mSpeakerEnabled = false;
            mUnidentifiedA0[side][i] = false;
        }
    }

    mSelectButtons[0].mContext = (void*)0;
    mSelectButtons[1].mContext = (void*)1;
    mSelectButtons[0].mSpeakerEnabled = false;
    mSelectButtons[1].mSpeakerEnabled = false;

    mRandomButtons[0].mContext = (void*)0;
    mRandomButtons[1].mContext = (void*)1;
    mRandomButtons[0].mSpeakerEnabled = false;
    mRandomButtons[1].mSpeakerEnabled = false;

    mUnidentified28[0] = -1;
    mUnidentified28[1] = -1;
    mSelectedSlots[0] = -1;
    mSelectedSlots[1] = -1;
    mUnidentified48[0] = false;
    mUnidentified48[1] = false;
    mDoneButton.mSpeakerEnabled = false;

    mBackButton.SetPopScene(false);

    if (GameInfoManager::Instance()->IsInMode3())
    {
        mTeams[0] = g_pCupManager->mPendingCupTeam;
        mTeams[1] = -1;
    }
    else if (GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mTeams[0] = GameInfoManager::Instance()->GetTeam(0);
        mTeams[1] = -1;
    }
    else
    {
        mTeams[0] = GameInfoManager::Instance()->GetTeam(0);
        mTeams[1] = GameInfoManager::Instance()->GetTeam(1);
    }

    for (int side = 0; side < 2; ++side)
    {
        for (i = 0; i < 8; ++i)
        {
            mAttributeImages[side][i] = 0;
            mPositionImages[side][i] = 0;
            mAttributesLoaded[side][i] = true;
            mPositionsLoaded[side][i] = true;
        }
    }
}

/**
 * Offset/Address/Size: 0x3F0 | 0x802288A8 | size: 0x198
 */
ChooseSidekicksSceneV2::~ChooseSidekicksSceneV2()
{
    TLInstance* timer = GetNavigationScene()->GetTimer();
    timer->m_bVisible = false;

    for (int i = 0; i < 8; ++i)
    {
        for (int side = 0; side < 2; ++side)
        {
            if (mAttributeImages[side][i] != 0)
            {
                delete mAttributeImages[side][i];
                mAttributeImages[side][i] = 0;
            }

            if (mPositionImages[side][i] != 0)
            {
                delete mPositionImages[side][i];
                mPositionImages[side][i] = 0;
            }
        }
    }
}

int ChooseSidekicksSceneV2::GetSide(unsigned long pad)
{
    if (mSidePads[0] == pad)
    {
        return 0;
    }

    if (mSidePads[1] == pad)
    {
        return 1;
    }

    return -1;
}


/**
 * Offset/Address/Size: 0x588 | 0x80228A40 | size: 0xF84
 */
void ChooseSidekicksSceneV2::SceneCreated()
{
    TLComponentInstance* sidekicks = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "SIDEKICKS");
    sidekicks->SetVisible(false);
    TLComponentInstance* cupSidekicks = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "SIDEKICKS RTSC");
    cupSidekicks->SetVisible(false);
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
    {
        FrontEndPresentation::GetInstance()->Call("StartStrikerCupCaptainHologramSequence");
        mSidekicksLayer = cupSidekicks;
    }
    else if (GameInfoManager::Instance()->IsOnline())
        mSidekicksLayer = cupSidekicks;
    else
        mSidekicksLayer = sidekicks;

    TLInstance* sidekick = FEFinder<TLInstance, -1>::Find<TLSlide>(mSidekicksLayer->GetActiveSlide(), "SIDEKICK");
    TLComponentInstance* pda[2];
    pda[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left");
    pda[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right");
    for (int i = 0; i < 8; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "SIDEKICK%d", i);
        mSidekickInstances[i] = (TLComponentInstance*)FEFinder<TLInstance, 4>::Find(sidekick, name);
    }
    for (int side = 0; side < 2; ++side)
    {
        TLComponentInstance* screens = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[side]->GetActiveSlide(), "pda_screens");
        TLComponentInstance* positions = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(screens->GetActiveSlide(), "positions");
        for (int i = 0; i < 3; ++i)
        {
            const char* texture;
            const char* click;
            switch (i)
            {
            case 0:
                texture = "01_dummy_texture_positions";
                click = "CLICK";
                break;
            case 1:
                texture = "02_dummy_texture_positions";
                click = "CLICK2";
                break;
            case 2:
                texture = "03_dummy_texture_positions";
                click = "CLICK3";
                break;
            default:
                continue;
            }
            mSlotInstances[side][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", texture);
            mSlotClickInstances[side][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", click);
            mGreenArrows[side] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "green_arrow");
            mGreenArrows[side]->SetVisible(false);
        }
        if (side == 0 && (GameInfoManager::Instance()->IsOnline() || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP))
        {
            FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "TITLE")->SetActiveSlide("team", true, false);
        }
    }
    mRandomButtonInstances[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "select button");
    mSelectButtonInstances[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "random button");
    mUnidentified50[0].Initialize(pda[0], 0);
    mRandomButtonInstances[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "select button");
    mSelectButtonInstances[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "random button");
    mUnidentified50[1].Initialize(pda[1], 1);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "button_ok")->SetVisible(false);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "button_ok")->SetVisible(false);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "SCREEN_TITLES")->SetActiveSlide("SIDEKICKS", true, false);

    TLComponentInstance* back = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        back = navigation->GetButton(4);
        mDoneButtonInstance = navigation->GetButton(0x20);
    }
    mUnidentified50[0].ReloadSidekicks();
    mUnidentified50[1].ReloadSidekicks();
    for (int side = 0; side < 2; ++side)
    {
        mCaptainComponents[side].Initialize(pda[side], side, 0);
        mCaptainComponents[side].SetDisplayMode(1);
        mCaptainComponents[side].SetReadyPromptVisible(false);
        mCaptainComponents[side].SetSidekickInfo(-1, 0, 0);
    }
    int captain;
    if (GameInfoManager::Instance()->IsInMode3())
        captain = CupManager::Instance()->GetPendingCupTeam();
    else
        captain = GameInfoManager::Instance()->GetTeam(0);
    mUnidentified50[0].Show();
    mUnidentified50[0].SetCaptain(captain);
    mCaptainComponents[0].ShowSlideIn();
    if (GameInfoManager::Instance()->IsOnline() || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
    {
        mCaptainComponents[1].SetVisible(false);
        mCaptainComponents[1].SetDisplayMode(5);
        LoadSidekickImages(captain, -1);
    }
    else
    {
        mUnidentified50[1].Show();
        mUnidentified50[1].SetCaptain(GameInfoManager::Instance()->GetTeam(1));
        mCaptainComponents[1].ShowSlideIn();
        LoadSidekickImages(captain, GameInfoManager::Instance()->GetTeam(1));
        FEMusic::StartStreamIfDifferent(2);
    }
    LoadSidekickTextures();
    ApplySidekickTextures();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    if (NetworkDraft::Instance()->fn_8022819C())
    {
        mDraftCountdown = NetworkDraft::Instance()->fn_8022ED88();
        UpdateDraftTimer(mDraftCountdown);
        mBackButton.Disable();
        back->SetVisible(false);
    }
    else
    {
        UpdateDraftTimer(-1);
        mBackButton.SetButtonInstance(back);
    }
}

/**
 * Offset/Address/Size: 0x150C | 0x802299C4 | size: 0x160
 */
void ChooseSidekicksSceneV2::UpdateDraftTimer(int value)
{
    TLSlide* slide = mPresentation->GetActiveSlide();
    FEFinder<TLInstance, 3>::Find<TLSlide>(slide,
        "Layer", "TimerText")
        ->m_bVisible = false;

    TLInstance* timer = GetNavigationScene()->GetTimer();
    if (value == -1)
    {
        timer->m_bVisible = false;
    }
    else
    {
        char text[8];
        timer->m_bVisible = true;
        nlSNPrintf(text, 8, "%d", value);
        nlStrToWcs(text, mTimerText, 8);
        ((TLTextInstance*)timer)->SetString(mTimerText);
    }
}


/**
 * Offset/Address/Size: 0x166C | 0x80229B24 | size: 0x1044
 */
void ChooseSidekicksSceneV2::Update(float dt)
{
    if (!AreImagesLoaded())
    {
        UpdateAsyncImages();
        return;
    }
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
    if (GameInfoManager::Instance()->IsOnline())
    {
        NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
        bool disconnected = false;
        for (int i = 0; i < roster->GetMachineCount(); ++i)
        {
            if (roster->GetMachineAid(i) == 0)
            {
                disconnected = true;
                break;
            }
        }
        if (disconnected)
        {
            ShowDisconnectedError();
            return;
        }
    }
    BaseSceneHandler::Update(dt);
    mCaptainComponents[0].Update(dt);
    mCaptainComponents[1].Update(dt);
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
        UpdateCharacterIdleAnimations(dt);
    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
        TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
        TLSlide* leftSlide = left->GetActiveSlide();
        TLSlide* rightSlide = right->GetActiveSlide();
        if (mState == 0 && !mPointerButtonsInitialized)
        {
            mUnidentified50[0].LoadSlotImages(dt);
            mUnidentified50[1].LoadSlotImages(dt);
        }
        if (leftSlide->GetCurrentTime() < leftSlide->GetStartTime() + leftSlide->GetDuration()
            || rightSlide->GetCurrentTime() < rightSlide->GetStartTime() + rightSlide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mState == 0)
        {
            if (!mPointerButtonsInitialized)
            {
                UpdateSlotButtons();
                if (GameInfoManager::Instance()->IsOnline())
                    GetNavigationScene()->SetButtons(0x20, true);
                else
                    GetNavigationScene()->SetButtons(0x24, true);
                for (int i = 0; i < 4; ++i)
                    GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
                InitializePointerButtons();
                mPointerButtonsInitialized = true;
                mState = 1;
            }
        }
        else if (mState == 2)
        {
            SubmitSidekickChoice();
            return;
        }
        else if (mState == 3)
        {
            if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
            {
                FrontEndPresentation::GetInstance()->Call("RemoveStrikerCupCaptainHologram");
                GameSceneManager::Instance()->Push((SceneList)6, SCREEN_BACK, true);
            }
            else
                GameSceneManager::Instance()->Push((SceneList)2, SCREEN_BACK, true);
            return;
        }
    }
    if (!mSidekickButtonsInitialized)
    {
        InitializeSidekickButtons();
        mSidekickButtonsInitialized = true;
    }
    if (mSelectionMade)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (g_pNetworkSessionBase->GetSessionMode() != 0 && !IsLocalDraftPad(i))
        {
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            continue;
        }
        if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            if (mSidePads[0] == i || mSidePads[1] == i)
            {
                ReleaseController(i);
            }
            continue;
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        for (int button = 0; button < 8; ++button)
            mSidekickButtons[button].HandlePointerEvent(&event);
        for (int side = 0; side < 2; ++side)
        {
            mSelectButtons[side].HandlePointerEvent(&event);
            mRandomButtons[side].HandlePointerEvent(&event);
            for (int slot = 0; slot < 3; ++slot)
            {
                mSlotButtons[side][slot].HandlePointerEvent(&event);
                mSlotClickButtons[side][slot].HandlePointerEvent(&event);
            }
        }
        mDoneButton.HandlePointerEvent(&event);
        if (mSelectionMade)
            return;
        if (!NetworkDraft::Instance()->fn_8022819C())
        {
            if (mBackButton.UpdateBackButton(event, dt))
            {
                mState = 3;
                GetNavigationScene()->HideButtons();
                TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
                TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
                left->SetActiveSlide("out", true, false);
                right->SetActiveSlide("out", true, false);
                if (mSidekicksShown)
                    mSidekicksLayer->SetActiveSlide("out", true, false);
                FEAudio::PlayAnimAudioEvent(0x9478C856, 0, 0, true);
                return;
            }
        }
        if ((mSidePads[0] == i || mSidePads[1] == i) && !g_pFEInput->IsConnected((eFEINPUT_PAD)i))
        {
            ReleaseController(i);
        }
    }
    mUnidentified50[0].LoadSlotImages(dt);
    mUnidentified50[1].LoadSlotImages(dt);
    UpdateSlotButtons();
    UpdatePointerCursors();
    ApplySidekickTextures();
    if (NetworkDraft::Instance()->fn_8022819C())
    {
        int countdown = NetworkDraft::Instance()->fn_8022ED88();
        if (mDraftCountdown != countdown)
        {
            mDraftCountdown = countdown;
            UpdateDraftTimer(countdown);
        }
        if (countdown == 0 && !mDraftExitDone)
        {
            CommitSidekickChoices();
            mState = 2;
            GetNavigationScene()->HideButtons();
            TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
            TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
            left->SetActiveSlide("out", true, false);
            right->SetActiveSlide("out", true, false);
            if (mSidekicksShown)
                mSidekicksLayer->SetActiveSlide("out", true, false);
            mDraftExitDone = true;
        }
    }
}
/**
 * Offset/Address/Size: 0x26B0 | 0x8022AB68 | size: 0x184
 */
void ChooseSidekicksSceneV2::OnSidekickPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mSelectedSlots[side] == -1)
    {
        return;
    }

    int slot = mSelectedSlots[side];
    mUnidentified50[side].SetSidekick(slot, lbl_8051D198[which]);
    mSlotInstances[side][slot]->SetActiveSlide("off", true, false);

    for (int i = 0; i < 3; ++i)
    {
        mSlotButtons[side][i].ResetPointerStates();
    }
    mSidekickButtons[which].SetPointerState(0, index);
    mCaptainComponents[side].SetDisplayMode(1);
    mGreenArrows[side]->m_bVisible = false;
    mUnidentified50[side].SetRecycleState(mSelectedSlots[side], 1);
    mSelectedSlots[side] = -1;
    mSidePads[side] = -1;
    FEAudio::PlayAnimAudioEvent(FECharacterSound::GetSidekickAcceptSound((eSidekickID)lbl_8051D198[which]), 0, 0, 1);
    GameInfoManager::Instance()->mRulesTable[GetTeam(side)].mValues[slot] = (eSidekickID)lbl_8051D198[which];
}

/**
 * Offset/Address/Size: 0x2834 | 0x8022ACEC | size: 0x1B8
 */
void ChooseSidekicksSceneV2::OnSidekickPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mSelectedSlots[side] == -1)
    {
        return;
    }

    mUnidentified28[side] = which;
    int sidekick = lbl_8051D198[which];
    mCaptainComponents[side].SetDisplayMode(3);
    mCaptainComponents[side].SetSidekickInfo(sidekick, index, 0);
    mGreenArrows[side]->m_bVisible = false;
    if (!mSidekickButtons[which].HasOtherPointerState(1, index))
    {
        mSidekickInstances[which]->SetActiveSlide("over", true, false);
    }
    mSidekickButtons[which].SetPointerState(1, index);
    mSidekickButtons[which].PlayHoverFeedback(index);

    switch (index)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0xA183DBCD, 0, 0, 1);
        break;
    case 1:
        FEAudio::PlayAnimAudioEvent(0xA183DBCE, 0, 0, 1);
        break;
    case 2:
        FEAudio::PlayAnimAudioEvent(0xA183DBCF, 0, 0, 1);
        break;
    case 3:
        FEAudio::PlayAnimAudioEvent(0xA183DBD0, 0, 0, 1);
        break;
    }
}

/**
 * Offset/Address/Size: 0x29EC | 0x8022AEA4 | size: 0xE0
 */
void ChooseSidekicksSceneV2::OnSidekickPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mSelectedSlots[side] == -1)
    {
        return;
    }

    if (!mSidekickButtons[which].HasOtherPointerState(1, index))
    {
        mSidekickInstances[which]->SetActiveSlide("off", true, false);
    }
    mSidekickButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x2ACC | 0x8022AF84 | size: 0x2B0
 */
void ChooseSidekicksSceneV2::OnSlotPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);
    if (mSidePads[group] != -1 || side != -1)
    {
        return;
    }

    mUnidentified48[group] = false;
    mCaptainComponents[group].SetReadyPromptVisible(false);
    if (!mSidekicksShown)
    {
        mSidekicksLayer->m_bVisible = true;
        mSidekicksLayer->SetActiveSlide("in", true, false);
        FEAudio::PlayAnimAudioEvent(0xDF52130F, 0, 0, 1);
        mSidekicksShown = true;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (i == slot)
        {
            mSlotInstances[group][i]->SetActiveSlide("down", true, false);
            mSlotClickInstances[group][i]->SetActiveSlide("down", true, false);
        }
        else
        {
            mSlotInstances[group][i]->SetActiveSlide("off", true, false);
            mSlotClickInstances[group][i]->SetActiveSlide("off", true, false);
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            mSlotButtons[group][i].SetPointerState(2, j);
        }
        for (int j = 0; j < 4; ++j)
        {
            mSlotClickButtons[group][i].SetPointerState(2, j);
        }
    }
    mSelectButtonInstances[group]->SetActiveSlide("off", true, false);
    mRandomButtonInstances[group]->SetActiveSlide("off", true, false);
    for (int i = 0; i < 4; ++i)
    {
        mSelectButtons[group].SetPointerState(0, i);
    }
    for (int i = 0; i < 4; ++i)
    {
        mRandomButtons[group].SetPointerState(0, i);
    }
    for (int i = 0; i < 4; ++i)
    {
        mDoneButton.SetPointerState(0, i);
    }
    mGreenArrows[group]->m_bVisible = true;
    mSidePads[group] = index;
    mSelectedSlots[group] = slot;
    mUnidentifiedA0[group][slot] = false;
    mUnidentified50[group].SetRecycleState(slot, 0);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
}

/**
 * Offset/Address/Size: 0x2D7C | 0x8022B234 | size: 0x1CC
 */
void ChooseSidekicksSceneV2::OnSlotPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mSidePads[group] != -1 || side != -1)
    {
        return;
    }

    if (!mSlotButtons[group][slot].HasOtherPointerState(1, index))
    {
        if (mUnidentifiedA0[group][slot])
        {
            mSlotClickInstances[group][slot]->SetActiveSlide("over", true, false);
        }
        else
        {
            mSlotInstances[group][slot]->SetActiveSlide("over", true, false);
            FEFinder<TLComponentInstance, 4>::FindOrDefault(mSlotInstances[group][slot],
                nlStringLowerHash("over"), nlStringLowerHash("CHANGE"), 0, 0, 0, 0)
                ->SetActiveSlide("Slide1", true, false);
        }

        FEAudio::PlayAnimAudioEvent(0x23628A1D, 0, 0, 1);
    }

    mSlotButtons[group][slot].SetPointerState(1, index);
    mSlotClickButtons[group][slot].SetPointerState(1, index);
    mSlotButtons[group][slot].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x2F48 | 0x8022B400 | size: 0x94
 */
void ChooseSidekicksSceneV2::OnSlotPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mSidePads[group] != -1 || side != -1)
    {
        return;
    }

    mSlotButtons[group][slot].SetPointerState(0, index);
    mSlotClickButtons[group][slot].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x2FDC | 0x8022B494 | size: 0x98
 */
void ChooseSidekicksSceneV2::OnSlotPointerInside(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mSidePads[group] != -1 || side != -1)
    {
        return;
    }

    if (mSlotButtons[group][slot].GetPointerState(index) != 0)
    {
        return;
    }

    OnSlotPointerEnter(index, context);
}

/**
 * Offset/Address/Size: 0x3074 | 0x8022B52C | size: 0xD4
 */
void ChooseSidekicksSceneV2::OnSelectPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    mSelectButtonInstances[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mSelectButtons[which].SetPointerState(2, i);
    }

    mUnidentified50[which].RandomizeSidekicks();
}

/**
 * Offset/Address/Size: 0x3148 | 0x8022B600 | size: 0xE8
 */
void ChooseSidekicksSceneV2::OnSelectPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    if (!mSelectButtons[which].HasOtherPointerState(1, index))
    {
        mSelectButtonInstances[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mSelectButtons[which].PlayHoverFeedback(index);
    mSelectButtons[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3230 | 0x8022B6E8 | size: 0xD8
 */
void ChooseSidekicksSceneV2::OnSelectPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    if (!mSelectButtons[which].HasOtherPointerState(1, index))
    {
        mSelectButtonInstances[which]->SetActiveSlide("off", true, false);
    }

    mSelectButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3308 | 0x8022B7C0 | size: 0xF4
 */
void ChooseSidekicksSceneV2::OnRandomPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    mRandomButtonInstances[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mRandomButtons[which].SetPointerState(2, i);
    }

    mCaptainComponents[which].SetReadyPromptVisible(false);
    mUnidentified48[which] = false;
    mUnidentified50[which].ResetSidekicks();
}

/**
 * Offset/Address/Size: 0x33FC | 0x8022B8B4 | size: 0xE8
 */
void ChooseSidekicksSceneV2::OnRandomPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    if (!mRandomButtons[which].HasOtherPointerState(1, index))
    {
        mRandomButtonInstances[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mRandomButtons[which].PlayHoverFeedback(index);
    mRandomButtons[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x34E4 | 0x8022B99C | size: 0xD8
 */
void ChooseSidekicksSceneV2::OnRandomPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1)
    {
        return;
    }

    if (!mRandomButtons[which].HasOtherPointerState(1, index))
    {
        mRandomButtonInstances[which]->SetActiveSlide("off", true, false);
    }

    mRandomButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x35BC | 0x8022BA74 | size: 0x370
 */
void ChooseSidekicksSceneV2::OnDonePointerPress(int index, void* context)
{
    if (mSidePads[0] != -1 || mSidePads[1] != -1)
    {
        return;
    }

    mDoneButtonInstance->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    for (int i = 0; i < 4; ++i)
    {
        mDoneButton.SetPointerState(0, i);
    }
    mDoneButton.Disable();
    mSelectionMade = true;
    mState = 2;
    GetNavigationScene()->HideButtons();
    TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide,
        "Layer", "PDA left");
    TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide,
        "Layer", "PDA right");
    left->SetActiveSlide("out", true, false);
    right->SetActiveSlide("out", true, false);
    CommitSidekickChoices();
    if (mSidekicksShown)
    {
        mSidekicksLayer->SetActiveSlide("out", true, false);
    }
    if (GameInfoManager::Instance()->mIsOnlineMode == 0 && !GameInfoManager::Instance()->IsInMode3())
    {
        FEAudio::PlayAnimAudioEvent(0x4861E03D, 0, 0, 1);
    }
    else if (GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        FEAudio::PlayAnimAudioEvent(0xD73F62C5, 0, 0, 1);
    }
}

/**
 * Offset/Address/Size: 0x392C | 0x8022BDE4 | size: 0xB0
 */
void ChooseSidekicksSceneV2::OnDonePointerEnter(int index, void* context)
{
    if (mSidePads[0] != -1 || mSidePads[1] != -1)
    {
        return;
    }

    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mDoneButton.SetPointerState(1, index);
    mDoneButton.PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x39DC | 0x8022BE94 | size: 0x8C
 */
void ChooseSidekicksSceneV2::OnDonePointerLeave(int index, void* context)
{
    if (mSidePads[0] != -1 || mSidePads[1] != -1)
    {
        return;
    }

    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("off", true, false);
    }

    mDoneButton.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3A68 | 0x8022BF20 | size: 0xCC
 */
void ChooseSidekicksSceneV2::OnDonePointerInside(int index, void* context)
{
    if (mSidePads[0] != -1 || mSidePads[1] != -1)
    {
        return;
    }

    if (mDoneButton.GetPointerState(index) == 0)
    {
        OnDonePointerEnter(index, context);
    }
}

/**
 * Offset/Address/Size: 0x3B34 | 0x8022BFEC | size: 0x734
 */
void ChooseSidekicksSceneV2::InitializeSidekickButtons()
{
    typedef Detail::MemFunImpl<void, void (ChooseSidekicksSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseSidekicksSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSidekickPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSidekickPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSidekickPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    TLComponentInstance* sidekicks;
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
        sidekicks = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "SIDEKICKS RTSC");
    else
        sidekicks = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "SIDEKICKS");
    TLComponentInstance* sidekick = FEFinder<TLComponentInstance, 4>::Find(sidekicks, "in", "SIDEKICK");
    feVector3 scale = sidekick->GetAssetScale();
    feVector3 position = sidekicks->GetAssetPosition();
    feVector3 sidekickPosition = sidekick->GetAssetPosition();
    for (int i = 0; i < 8; ++i)
    {
        char name[8];
        nlSNPrintf(name, sizeof(name), "%d", i);
        char imageName[32];
        nlSNPrintf(imageName, sizeof(imageName), "%02d_dummy_texture", i);
        TLInstance* layer = FEFinder<TLInstance, 1>::Find(mSidekickInstances[i], "off", name);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(layer, imageName);
        feVector3 itemPosition = mSidekickInstances[i]->GetAssetPosition();
        float x = position.f.x + sidekickPosition.f.x;
        float y = position.f.y + sidekickPosition.f.y;
        x += itemPosition.f.x * scale.f.x;
        y += itemPosition.f.y * scale.f.y;
        mSidekickButtons[i].SetInstanceBounds(image, false, x, y, 1.0f, 1.0f);
        mSidekickButtons[i].SetPointerEnterCallback(enter);
        mSidekickButtons[i].SetPointerLeaveCallback(leave);
        mSidekickButtons[i].SetPointerPressCallback(press);
    }
}

/**
 * Offset/Address/Size: 0x4268 | 0x8022C720 | size: 0xED8
 */
void ChooseSidekicksSceneV2::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (ChooseSidekicksSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseSidekicksSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback selectEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSelectPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSelectPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSelectPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnRandomPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnRandomPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnRandomPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneInside(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnDonePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback donePress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSlotPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSlotPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotInside(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSlotPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::OnSlotPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, 0);
    mDoneButton.SetPointerEnterCallback(doneEnter);
    mDoneButton.SetPointerLeaveCallback(doneLeave);
    mDoneButton.SetPointerInsideCallback(doneInside);
    mDoneButton.SetPointerPressCallback(donePress);
    for (int side = 0; side < 2; ++side)
    {
        if ((GameInfoManager::Instance()->mIsOnlineMode || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP) && side == 1)
            continue;
        mSelectButtons[side].SetInstanceBounds(mSelectButtonInstances[side], true, -157.0f, 159.0f, 0.7f, 0.7f);
        mSelectButtons[side].SetPointerEnterCallback(selectEnter);
        mSelectButtons[side].SetPointerLeaveCallback(selectLeave);
        mSelectButtons[side].SetPointerPressCallback(selectPress);
        mRandomButtons[side].SetInstanceBounds(mRandomButtonInstances[side], true, -157.0f, 159.0f, 0.8f, 0.7f);
        mRandomButtons[side].SetPointerEnterCallback(randomEnter);
        mRandomButtons[side].SetPointerLeaveCallback(randomLeave);
        mRandomButtons[side].SetPointerPressCallback(randomPress);
        for (int i = 0; i < 3; ++i)
        {
            TLInstance* instance = FEFinder<TLInstance, 4>::Find(mSlotInstances[side][i], mSlotInstances[side][i]->m_szName);
            float x = mSlotInstances[side][i]->GetAssetPosition().f.x;
            float y = mSlotInstances[side][i]->GetAssetPosition().f.y;
            mSlotButtons[side][i].SetInstanceBounds(instance, true,
                x + lbl_804E8530[side].x, y + lbl_804E8530[side].y, 1.0f, 1.0f);
            mSlotButtons[side][i].SetPointerEnterCallback(slotEnter);
            mSlotButtons[side][i].SetPointerLeaveCallback(slotLeave);
            mSlotButtons[side][i].SetPointerPressCallback(slotPress);
            mSlotButtons[side][i].SetPointerInsideCallback(slotInside);
            mSlotClickButtons[side][i].SetInstanceBounds(mSlotClickInstances[side][i], true,
                lbl_804E8530[side].x, lbl_804E8530[side].y, 1.0f, 1.0f);
            mSlotClickButtons[side][i].SetPointerEnterCallback(slotEnter);
            mSlotClickButtons[side][i].SetPointerLeaveCallback(slotLeave);
            mSlotClickButtons[side][i].SetPointerPressCallback(slotPress);
            mSlotClickButtons[side][i].SetPointerInsideCallback(slotInside);
        }
    }
}

/**
 * Offset/Address/Size: 0x5140 | 0x8022D5F8 | size: 0x2AC
 */
void ChooseSidekicksSceneV2::UpdatePointerCursors()
{
    bool idle[2] = { true, true };
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mSidePads[0] != -1 && mSidePads[0] != i && mSidePads[1] != -1 && mSidePads[1] != i)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (g_pNetworkSessionBase->GetSessionMode() != 0 && !IsLocalDraftPad(i))
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else
        {
            pointer->SetActiveSlide("cursor", true, false);
            for (int j = 0; j < 8; ++j)
            {
                if (mSidekickButtons[j].GetPointerState(i) == 1)
                {
                    if (mSidePads[0] == i)
                    {
                        idle[0] = false;
                    }
                    else if (mSidePads[1] == i)
                    {
                        idle[1] = false;
                    }
                    break;
                }
            }
        }
    }
    for (int side = 0; side < 2; ++side)
    {
        if (mSidePads[side] != -1 && idle[side])
        {
            mUnidentified28[side] = -1;
            mCaptainComponents[side].SetDisplayMode(1);
            mGreenArrows[side]->m_bVisible = true;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (!mSlotButtons[side][i].HasOtherPointerState(1, -1) && !mSlotButtons[side][i].HasOtherPointerState(2, -1))
            {
                mSlotInstances[side][i]->SetActiveSlide("off", true, false);
                mSlotClickInstances[side][i]->SetActiveSlide("off", true, false);
            }
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        if (!mSidekickButtons[i].HasOtherPointerState(1, -1))
        {
            mSidekickInstances[i]->SetActiveSlide("off", true, false);
        }
    }
}

/**
 * Offset/Address/Size: 0x53EC | 0x8022D8A4 | size: 0x188
 */
void ChooseSidekicksSceneV2::LoadSidekickTextures()
{
    for (int i = 0; i < 8; ++i)
    {
        const CharacterInfo& sidekick = GetCharacterInfo(GetCharacterIndexFromSidekick(lbl_8051D198[i]));
        const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(0));
        char selected[64];
        char disabled[64];
        nlSNPrintf(selected, 64, "sidekick_%s_%s_s", sidekick.mName, captain.mName);
        nlSNPrintf(disabled, 64, "sidekick_%s_%s_ds", sidekick.mName, captain.mName);
        TLImageInstance* selectedImage = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation,
            "art", "Layer", selected);
        TLImageInstance* disabledImage = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation,
            "art", "Layer", disabled);
        mSidekickTextures[i][0] = selectedImage->m_pTextureResource;
        mSidekickTextures[i][1] = disabledImage->m_pTextureResource;
    }
}

/**
 * Offset/Address/Size: 0x5574 | 0x8022DA2C | size: 0x144
 */
void ChooseSidekicksSceneV2::ApplySidekickTextures()
{
    for (int i = 0; i < 8; ++i)
    {
        FETextureResource* texture = mSidekickTextures[i][0];
        char name[64];
        char index[8];
        nlSNPrintf(name, 64, "%02d_dummy_texture", i);
        nlSNPrintf(index, 8, "%d", i);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(mSidekickInstances[i]->GetActiveSlide(),
            index, name);
        if (image != 0 && texture != 0)
        {
            image->m_pTextureResource = texture;
        }
    }
}

/**
 * Offset/Address/Size: 0x56B8 | 0x8022DB70 | size: 0xF8
 */
void ChooseSidekicksSceneV2::CommitSidekickChoices()
{
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
    {
        GameRules rules;
        rules.mValues[0] = (eSidekickID)mUnidentified50[0].GetSidekick(0);
        rules.mValues[1] = (eSidekickID)mUnidentified50[0].GetSidekick(1);
        rules.mValues[2] = (eSidekickID)mUnidentified50[0].GetSidekick(2);
        g_pCupManager->SetSidekicks(rules);
    }
    else
    {
        for (int side = 0; side < 2; ++side)
        {
            for (int slot = 0; slot < 3; ++slot)
            {
                int sidekick = mUnidentified50[side].GetSidekick(slot);
                GameInfoManager::Instance()->SetSidekick(side, sidekick, slot);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x57B0 | 0x8022DC68 | size: 0x148
 */
void ChooseSidekicksSceneV2::SubmitSidekickChoice()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->IsOnline())
    {
        NetworkDraft::Instance()->SendSidekickChoice();
    }
    else if (gameInfo->IsInMode3())
    {
        mUnidentified48[0] = false;
        mCaptainComponents[0].SetReadyPromptVisible(false);
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, true);
        popup->Create((ePopupMenu)0x39, Function<FnVoidVoid>(fn_8022E0C0), Function<FnVoidVoid>(fn_8022E18C));
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)0x4, SCREEN_FORWARD, true);
    }
}

/**
 * Offset/Address/Size: 0x58F8 | 0x8022DDB0 | size: 0x310
 */
void ChooseSidekicksSceneV2::UpdateSlotButtons()
{
    for (int side = 0; side < 2; ++side)
    {
        for (int i = 0; i < 3; ++i)
        {
            TLComponentInstance* recycle = FEFinder<TLComponentInstance, 4>::FindOrDefault(mSlotInstances[side][i],
                "off", "recycle");
            if (mUnidentifiedA0[side][i])
            {
                mSlotClickButtons[side][i].Enable();
                mSlotClickInstances[side][i]->m_bVisible = true;
                mSlotButtons[side][i].Disable();
                recycle->m_bVisible = false;
            }
            else
            {
                mSlotClickButtons[side][i].Disable();
                mSlotClickInstances[side][i]->m_bVisible = false;
                mSlotButtons[side][i].Enable();
                recycle->m_bVisible = true;
            }
        }
    }

    if (mSidePads[0] == -1 && mSidePads[1] == -1)
    {
        mDoneButton.Enable();
        mDoneButtonInstance->m_bVisible = true;
    }
    else
    {
        mDoneButton.Disable();
        mDoneButtonInstance->m_bVisible = false;
    }
}

/**
 * Offset/Address/Size: 0x5C08 | 0x8022E0C0 | size: 0xCC
 */
static void fn_8022E0C0()
{
    g_pCupManager->mForceHighestSkillLevel = false;
    g_pCupManager->StartCupSeries();
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
    }
    CupNewsScene* news = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)0x27, SCREEN_NOTHING, false);
    news->SetDisplayMode(0);
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    SaveLoad::StartSave(false);
}

/**
 * Offset/Address/Size: 0x5CD4 | 0x8022E18C | size: 0xCC
 */
static void fn_8022E18C()
{
    g_pCupManager->mForceHighestSkillLevel = true;
    g_pCupManager->StartCupSeries();
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
    }
    CupNewsScene* news = (CupNewsScene*)GameSceneManager::Instance()->Push((SceneList)0x27, SCREEN_NOTHING, false);
    news->SetDisplayMode(0);
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    SaveLoad::StartSave(false);
}

/**
 * Offset/Address/Size: 0x5DA0 | 0x8022E258 | size: 0x874
 */
void ChooseSidekicksSceneV2::LoadSidekickImages(int firstCaptain, int secondCaptain)
{
    FEPresentation* presentation = GameSceneManager::Instance()->GetCurrentScene()->GetPresentation();
    bool firstAlt = false;
    bool secondAlt = false;
    TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(presentation->m_currentSlide, "Layer", "PDA left");
    TLComponentInstance* attributes = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(left->GetActiveSlide(), "attributes_sidekicks");
    TLComponentInstance* firstPda = FEFinder<TLComponentInstance, 4>::Find(attributes, "Slide1", "attributes_sidekicks", "sidekicks_pda");
    TLComponentInstance* secondPda = 0;
    if (secondCaptain != -1)
    {
        firstAlt = CaptainsNeedAlternateColour(firstCaptain, secondCaptain);
        secondAlt = CaptainsNeedAlternateColour(secondCaptain, firstCaptain);
        TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(presentation->m_currentSlide, "Layer", "PDA right");
        TLComponentInstance* attributes = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(right->GetActiveSlide(), "attributes_sidekicks");
        secondPda = FEFinder<TLComponentInstance, 4>::Find(attributes, "Slide1", "attributes_sidekicks", "sidekicks_pda");
    }

    for (int i = 0; i < 8; ++i)
    {
        mPositionImages[0][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
        const CharacterInfo& sidekick = GetCharacterInfo(GetCharacterIndexFromSidekick(i));
        const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(firstCaptain));
        char positionName[64];
        nlSNPrintf(positionName, sizeof(positionName), "position_%s_mario", sidekick.mName);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, "art", "layer", positionName);
        char positionPath[64];
        if (firstAlt)
            nlSNPrintf(positionPath, sizeof(positionPath), "fe/sidekick_images/position_%s_%s_alt", sidekick.mName, captain.mName);
        else
            nlSNPrintf(positionPath, sizeof(positionPath), "fe/sidekick_images/position_%s_%s", sidekick.mName, captain.mName);
        mPositionImages[0][i]->SetImageInstance(image);
        mPositionImages[0][i]->QueueLoad(positionPath, false);
        mPositionsLoaded[0][i] = false;

        char attributeName[32];
        nlSNPrintf(attributeName, sizeof(attributeName), "attributes_%s_mario", sidekick.mName);
        mAttributeImages[0][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
        TLImageInstance* attributeImage = FEFinder<TLImageInstance, 2>::Find(firstPda, sidekick.mName, attributeName);
        char attributePath[64];
        if (firstAlt)
            nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s_alt", sidekick.mName, captain.mName);
        else
            nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s", sidekick.mName, captain.mName);
        mAttributeImages[0][i]->SetImageInstance(attributeImage);
        mAttributeImages[0][i]->QueueLoad(attributePath, false);
        mAttributesLoaded[0][i] = false;

        if (secondCaptain != -1)
        {
            mPositionImages[1][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
            const CharacterInfo& sidekick = GetCharacterInfo(GetCharacterIndexFromSidekick(i));
            const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(secondCaptain));
            char positionName[64];
            nlSNPrintf(positionName, sizeof(positionName), "position_%s_luigi", sidekick.mName);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, "art", "layer", positionName);
            char positionPath[64];
            if (secondAlt)
                nlSNPrintf(positionPath, sizeof(positionPath), "fe/sidekick_images/position_%s_%s_alt", sidekick.mName, captain.mName);
            else
                nlSNPrintf(positionPath, sizeof(positionPath), "fe/sidekick_images/position_%s_%s", sidekick.mName, captain.mName);
            mPositionImages[1][i]->SetImageInstance(image);
            mPositionImages[1][i]->QueueLoad(positionPath, false);
            mPositionsLoaded[1][i] = false;

            char attributeName[32];
            nlSNPrintf(attributeName, sizeof(attributeName), "attributes_%s_luigi", sidekick.mName);
            mAttributeImages[1][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
            TLImageInstance* attributeImage = FEFinder<TLImageInstance, 2>::Find(secondPda, sidekick.mName, attributeName);
            char attributePath[64];
            if (secondAlt)
                nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s_alt", sidekick.mName, captain.mName);
            else
                nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s", sidekick.mName, captain.mName);
            mAttributeImages[1][i]->SetImageInstance(attributeImage);
            mAttributeImages[1][i]->QueueLoad(attributePath, false);
            mAttributesLoaded[1][i] = false;
        }
    }
}

/**
 * Offset/Address/Size: 0x6614 | 0x8022EACC | size: 0xEC
 */
void ChooseSidekicksSceneV2::UpdateAsyncImages()
{
    for (int i = 0; i < 8; ++i)
    {
        if (!mPositionsLoaded[0][i] && mPositionImages[0][i] != 0)
        {
            mPositionsLoaded[0][i] = mPositionImages[0][i]->Update(true);
        }
        if (!mAttributesLoaded[0][i] && mAttributeImages[0][i] != 0)
        {
            mAttributesLoaded[0][i] = mAttributeImages[0][i]->Update(true);
        }
        if (!mPositionsLoaded[1][i] && mPositionImages[1][i] != 0)
        {
            mPositionsLoaded[1][i] = mPositionImages[1][i]->Update(true);
        }
        if (!mAttributesLoaded[1][i] && mAttributeImages[1][i] != 0)
        {
            mAttributesLoaded[1][i] = mAttributeImages[1][i]->Update(true);
        }
    }
}

/**
 * Offset/Address/Size: 0x6700 | 0x8022EBB8 | size: 0x168
 */
bool ChooseSidekicksSceneV2::AreImagesLoaded()
{
    if (mImagesLoaded)
    {
        return true;
    }
    for (int side = 0; side < 2; ++side)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (!mPositionsLoaded[side][i] || !mAttributesLoaded[side][i])
            {
                return false;
            }
        }
    }
    mImagesLoaded = true;
    FEAudio::PlayAnimAudioEvent(0x902BD189, 0, 0, 1);
    return true;
}

/**
 * Offset/Address/Size: 0x6868 | 0x8022ED20 | size: 0x5C
 */
void ChooseSidekicksSceneV2::OnDisconnectDismissed()
{
    mPopupActive = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_BACK, true);
}

void ChooseSidekicksSceneV2::ReleaseController(int index)
{
    bool side = mSidePads[0] != index;
    mSlotInstances[side][mSelectedSlots[side]]->SetActiveSlide("off", true, false);
    for (int slot = 0; slot < 3; ++slot)
    {
        mSlotButtons[side][slot].ResetPointerStates();
    }
    mCaptainComponents[side].SetDisplayMode(1);
    mGreenArrows[side]->m_bVisible = false;
    mUnidentified50[side].SetRecycleState(mSelectedSlots[side], 1);
    mSelectedSlots[side] = -1;
    mSidePads[side] = -1;
}

inline void ChooseSidekicksSceneV2::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    NetworkDraft::Instance()->UnregisterMessageReceivers();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(Bind<void>(MemFun(&ChooseSidekicksSceneV2::OnDisconnectDismissed), this)));
        mPopupActive = true;
    }
}
