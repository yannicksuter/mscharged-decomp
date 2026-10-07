#include "Game/SH/SHChooseCaptains.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/CaptainSelectionOrder.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePointer.inl"
#include "Game/FE/fePresentation.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Team.h"
#include "Game/GameInfo.h"
#include "Game/GameSceneManager.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/SH/SHOnlineMatchmakingDraft.h"
#include "Game/NetworkLobby.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"

static int sCaptainButtonSelectionOrder[12] = { 0, 5, 3, 6, 4, 7, 1, 8, 2, 9, 10, 11 };

/**
 * Offset/Address/Size: 0x0 | 0x80222098 | size: 0x468
 */
ChooseCaptainsSceneV2::ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement)
    : mPopupActive(false)
    , mSceneType(sceneType)
    , mMovement(movement)
    , mCaptainButtonsInitialized(false)
    , mPointerButtonsInitialized(false)
    , mCaptainsShown(false)
    , mBothConfirmed(false)
    , mInputSuppressed(false)
    , mDoneButtonInstance(0)
    , mDraftExitDone(false)
    , mScenePhase(PHASE_ENTERING)
{
    mSidePads[0] = -1;
    mSidePads[1] = -1;
    mSelectDisplays[0] = 0;
    mSelectDisplays[1] = 0;

    mSelectButtons[0].mContext = (void*)0;
    mSelectButtons[0].mSpeakerEnabled = false;
    mSelectButtons[1].mContext = (void*)1;
    mSelectButtons[1].mSpeakerEnabled = false;

    mDoneButton.mSpeakerEnabled = false;

    mReadyButtons[0].mContext = (void*)0;
    mReadyButtons[0].Disable();
    mReadyButtons[1].mContext = (void*)1;
    mReadyButtons[1].Disable();

    mSelectedCaptains[0] = -1;
    mSelectedCaptains[1] = -1;
    mConfirmed[0] = false;
    if (GameInfoManager::Instance()->IsInMode3() || GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mConfirmed[1] = true;
    }
    else
    {
        mConfirmed[1] = false;
    }
    mReadyPressed[0] = false;
    mReadyPressed[1] = false;
    mChangeTextShown[0] = false;
    mChangeTextShown[1] = false;
    mCaptainIds[0] = -1;
    mCaptainIds[1] = -1;
    mSideJoined[0] = movement == SCREEN_BACK;
    mSideJoined[1] = movement == SCREEN_BACK;

    mBackButton.SetPopScene(false);
    if (mSceneType == ST_STRIKER_CUP)
    {
        mBackButton.SetPushBackScene(false);
    }

    if (movement == SCREEN_BACK)
    {
        if (mSceneType == ST_STRIKER_CUP)
        {
            int team = g_pCupManager->mPendingCupTeam;
            mConfirmed[0] = true;
            mCaptainIds[0] = team;
        }
        else
        {
            mCaptainIds[0] = GameInfoManager::Instance()->GetTeam(0);
            mConfirmed[0] = true;
            if (GameInfoManager::Instance()->mIsOnlineMode == 0)
            {
                int team = GameInfoManager::Instance()->GetTeam(1);
                mConfirmed[1] = true;
                mCaptainIds[1] = team;
            }
        }
    }

    for (int i = 0; i < 12; ++i)
    {
        mCaptainButtons[i].mContext = (void*)i;
        mCaptainButtons[i].mSpeakerEnabled = false;

        if (mCaptainIds[0] == sCaptainButtonSelectionOrder[i])
        {
            mSelectedCaptains[0] = i;
        }
        else if (mCaptainIds[1] == sCaptainButtonSelectionOrder[i])
        {
            mSelectedCaptains[1] = i;
        }
    }
}

/**
 * Offset/Address/Size: 0x468 | 0x80222500 | size: 0xE8
 */
ChooseCaptainsSceneV2::~ChooseCaptainsSceneV2()
{
    TLInstance* timer = GetNavigationScene()->GetTimer();
    timer->m_bVisible = false;
}

void ChooseCaptainsSceneV2::SceneCreated()
{
    mCaptainsLayer = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "CAPTAINS");
    mCaptainsLayer->SetVisible(false);

    TLInstance* captains = FEFinder<TLInstance, -1>::Find<TLSlide>(mCaptainsLayer->GetActiveSlide(), "captains");
    for (int i = 0; i < 12; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "captain%d", i);
        mCaptainInstances[i] = (TLComponentInstance*)FEFinder<TLInstance, TLAT_COMPONENT>::Find(captains, name);
    }

    mPDALayers[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left");
    mPDALayers[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right");
    mSelectDisplays[0] = mPDALayers[0];
    mSelectDisplays[1] = mPDALayers[1];

    mSelectButtonInstances[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[0]->GetActiveSlide(), "select button");
    mOkButtonInstances[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[0]->GetActiveSlide(), "button_ok");
    mOkButtonInstances[0]->SetVisible(false);
    TLComponentInstance* screens = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[0]->GetActiveSlide(), "pda_screens");
    mGreenArrows[0] = FEFinder<TLInstance, TLAT_COMPONENT>::Find(screens, "empty", "green_arrow");
    mGreenArrows[0]->SetVisible(false);

    TLComponentInstance* back = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        back = navigation->GetButton(4);
        mDoneButtonInstance = navigation->GetButton(0x20);
    }
    mDoneButtonInstance->SetActiveSlide("off", true, false);

    TLComponentInstance* cupPda = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right RTSC");
    mCupPDALayer = cupPda;
    if (mSceneType != ST_STRIKER_CUP)
    {
        mSelectButtonInstances[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[1]->GetActiveSlide(), "select button");
        mOkButtonInstances[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[1]->GetActiveSlide(), "button_ok");
        mOkButtonInstances[1]->SetVisible(false);
        screens = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mPDALayers[1]->GetActiveSlide(), "pda_screens");
        mGreenArrows[1] = FEFinder<TLInstance, TLAT_COMPONENT>::Find(screens, "empty", "green_arrow");
        mGreenArrows[1]->SetVisible(false);
        cupPda->SetVisible(false);
        mCaptainComponents[1].Initialize(mPDALayers[1], 1, 0);
    }
    else
    {
        mCaptainComponents[1].Initialize(cupPda, 1, 0);
        mPDALayers[1]->SetVisible(false);
        TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(cupPda->GetActiveSlide(), "scrollbar");
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(scrollbar->GetActiveSlide(), "up_arrow")->SetActiveSlide("unused", true, false);
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(scrollbar->GetActiveSlide(), "down_arrow")->SetActiveSlide("unused", true, false);
        FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault<TLSlide>(scrollbar->GetActiveSlide(), "track", "btn_scroll_minmax")->SetVisible(false);
    }

    mCaptainComponents[0].Initialize(mPDALayers[0], 0, 0);
    for (int side = 0; side < 2; ++side)
    {
        mCaptainComponents[side].SetDisplayMode(0);
        mCaptainComponents[side].SetReadyPromptVisible(false);
        mCaptainComponents[side].SetCaptainInfo(mCaptainIds[side], 0, 1);
    }
    mCaptainComponents[0].ShowSlideIn();
    UpdateSelectText(0);
    mSelectButtonInstances[0]->SetVisible(false);

    TLComponentInstance* in = (TLComponentInstance*)FEFinder<TLInstance, TLAT_COMPONENT>::Find(mPDALayers[0], "in", "pda_screens");
    TLInstance* attributes = FEFinder<TLInstance, TLAT_COMPONENT>::Find(in, "Slide1", "attributes_captains");
    TLComponentInstance* title = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(((TLComponentInstance*)attributes)->GetActiveSlide(), "attributes_captains", "TITLE");
    if (GameInfoManager::Instance()->IsOnline())
    {
        title->SetActiveSlide("captain", true, false);
        mCaptainComponents[1].ShowSlideIn();
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right", "OFF", "clickable_AWAY", "CHOOSE AWAY CAPTAIN")->SetVisible(false);
        mSelectButtonInstances[1]->SetVisible(false);
    }
    else if (mSceneType == ST_STRIKER_CUP)
    {
        title->SetActiveSlide("captain", true, false);
        mCaptainComponents[1].ShowSlideIn();
    }
    else
    {
        mCaptainComponents[1].ShowSlideIn();
        mSelectButtonInstances[1]->SetVisible(false);
        UpdateSelectText(1);
        FEMusic::StartStreamIfDifferent(2);
    }

    LoadCaptainTextures();
    RefreshCaptainImages();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    if (NetworkDraft::Instance()->IsDraftActive())
    {
        mDraftCountdown = NetworkDraft::Instance()->GetCaptainDraftCountdown();
        UpdateDraftTimer(mDraftCountdown);
        mBackButton.Disable();
        back->SetVisible(false);
    }
    else
    {
        UpdateDraftTimer(-1);
        mBackButton.SetButtonInstance(back);
    }

    if (mMovement == SCREEN_BACK)
    {
        mSelectDisplays[0] = mSelectButtonInstances[0];
        mSelectButtonInstances[0]->SetVisible(true);
        if (!GameInfoManager::Instance()->IsInMode3() && !GameInfoManager::Instance()->IsOnline())
        {
            mCaptainComponents[0].ApplyCaptainColours(mCaptainIds[0], mCaptainIds[1]);
            mCaptainComponents[1].ApplyCaptainColours(mCaptainIds[1], mCaptainIds[0]);
            mSelectDisplays[1] = mSelectButtonInstances[1];
            mSelectButtonInstances[1]->SetVisible(true);
        }
    }

    FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "Description_clip")->SetVisible(false);
    char states[2][8] = { "off", "over" };
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* choose = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left", states[i], "clickable", "CHOOSE CAPTAIN");
        TLComponentInstance* home = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left", states[i], "clickable", "CHOOSE HOME CAPTAIN");
        if (mSceneType == ST_STRIKER_CUP || GameInfoManager::Instance()->IsOnline())
        {
            choose->SetVisible(true);
            home->SetVisible(false);
        }
        else
        {
            choose->SetVisible(false);
            home->SetVisible(true);
        }
    }

    TLComponentInstance* titles = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "SCREEN_TITLES");
    TLInstance* captain = FEFinder<TLInstance, TLAT_TEXT>::Find<TLSlide>(titles->GetActiveSlide(), "CAPTAIN");
    TLInstance* captainsTitle = FEFinder<TLInstance, TLAT_TEXT>::Find<TLSlide>(titles->GetActiveSlide(), "CAPTAINS");
    if (mSceneType == ST_STRIKER_CUP || GameInfoManager::Instance()->IsOnline())
        captainsTitle->SetVisible(false);
    else
        captain->SetVisible(false);
}

/**
 * Offset/Address/Size: 0x1B00 | 0x80223B98 | size: 0x170
 */
void ChooseCaptainsSceneV2::UpdateDraftTimer(int countdown)
{
    TLSlide* slide = mPresentation->GetActiveSlide();

    FEFinder<TLTextInstance, 3>::FindOrDefault<TLSlide>(slide, "Layer", "TimerText")->m_bVisible = false;

    TLInstance* timer = GetNavigationScene()->GetTimer();

    if (countdown == -1)
    {
        timer->m_bVisible = false;
    }
    else
    {
        char text[8];

        timer->m_bVisible = true;
        nlSNPrintf(text, 8, "%d", countdown);
        nlStrToWcs(text, mTimerText, 8);
        ((TLTextInstance*)timer)->SetString(mTimerText);
    }
}

/**
 * Offset/Address/Size: 0x1C70 | 0x80223D08 | size: 0x80
 */
bool IsLocalDraftPad(int pad)
{
    if (IsOnlineRankedMatch())
    {
        return pad == gOnlineLocalControllerIndices[0];
    }

    if (NetworkDraft::Instance()->mCurrentDrafterIsGuest)
    {
        return pad == gOnlineLocalControllerIndices[1];
    }

    return pad == gOnlineLocalControllerIndices[0];
}

void ChooseCaptainsSceneV2::Update(float dt)
{
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
    if (mScenePhase == PHASE_ENTERING || mScenePhase == PHASE_EXITING_FORWARD || mScenePhase == PHASE_EXITING_BACK)
    {
        TLSlide* leftSlide = mPDALayers[0]->GetActiveSlide();
        TLSlide* rightSlide = mPDALayers[1]->GetActiveSlide();
        if (leftSlide->GetCurrentTime() < leftSlide->GetStartTime() + leftSlide->GetDuration()
            || rightSlide->GetCurrentTime() < rightSlide->GetStartTime() + rightSlide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mScenePhase == PHASE_ENTERING)
        {
            if (GameInfoManager::Instance()->IsOnline())
                GetNavigationScene()->SetButtons(0x20, true);
            else
                GetNavigationScene()->SetButtons(0x24, true);
            UpdateDoneButton();
            if (!mSideJoined[0])
                mPDALayers[0]->SetActiveSlide("off", true, false);
            if (!mSideJoined[1])
                mPDALayers[1]->SetActiveSlide("off", true, false);
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
            if (mMovement == SCREEN_BACK && mSceneType == ST_STRIKER_CUP)
            {
                int captain = g_pCupManager->GetPendingCupTeam();
                mCaptainComponents[1].SetDisplayMode(6);
                mCaptainComponents[1].SetCaptainInfo(captain, 0, 1);
            }
            mScenePhase = PHASE_CHOOSING;
            InitializePointerButtons();
            mPointerButtonsInitialized = true;
        }
        else if (mScenePhase == PHASE_EXITING_FORWARD)
        {
            if (mSceneType == ST_STRIKER_CUP)
                GameSceneManager::Instance()->Push(SCENE_CHOOSE_SIDEKICKS_STRIKER_CUP, SCREEN_FORWARD, true);
            else
            {
                if (GameInfoManager::Instance()->IsOnline())
                    NetworkDraft::Instance()->SendCaptainChoice();
                GameSceneManager::Instance()->Push(SCENE_CHOOSE_SIDEKICKS_DOMINATION, SCREEN_FORWARD, true);
            }
            return;
        }
        else if (mScenePhase == PHASE_EXITING_BACK)
        {
            if (GameInfoManager::Instance()->IsOnline())
                GameSceneManager::Instance()->Push(SCENE_ONLINE_MENU, SCREEN_BACK, true);
            else if (mSceneType != ST_STRIKER_CUP)
                GameSceneManager::Instance()->Push(SCENE_GAMEPLAY_OPTIONS, SCREEN_BACK, true);
            if (mSceneType == ST_STRIKER_CUP)
            {
                GameSceneManager::Instance()->Pop();
                g_pCupManager->SetMode(-1);
                FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
                FrontEndPresentation::GetInstance()->Call("TransitionStrikerCupToMainMenu");
            }
            else
                FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, true);
            return;
        }
    }
    if (!mCaptainButtonsInitialized)
    {
        InitializeCaptainButtons();
        mCaptainButtonsInitialized = true;
        return;
    }
    if (mInputSuppressed)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (g_pNetworkSessionBase->GetSessionMode() != 0 && !IsLocalDraftPad(i))
        {
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            continue;
        }
        if (mSceneType == ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            if (mSidePads[0] == i || mSidePads[1] == i)
                ReleaseController(i);
            continue;
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        for (int button = 0; button < 12; ++button)
            mCaptainButtons[button].HandlePointerEvent(&event);
        for (int side = 0; side < 2; ++side)
            mSelectButtons[side].HandlePointerEvent(&event);
        mDoneButton.HandlePointerEvent(&event);
        if (mInputSuppressed)
            return;
        if (!NetworkDraft::Instance()->IsDraftActive())
        {
            if (mBackButton.UpdateBackButton(event, dt))
            {
                mScenePhase = PHASE_EXITING_BACK;
                GetNavigationScene()->HideButtons();
                mPDALayers[0]->SetActiveSlide("out", true, false);
                mPDALayers[1]->SetActiveSlide("out", true, false);
                mCupPDALayer->SetActiveSlide("out", true, false);
                if (mCaptainsShown)
                    mCaptainsLayer->SetActiveSlide("out", true, false);
                return;
            }
        }
        if ((mSidePads[0] == i || mSidePads[1] == i) && !g_pFEInput->IsConnected((eFEINPUT_PAD)i))
            ReleaseController(i);
    }
    UpdateDoneButton();
    UpdatePointerCursors();
    RefreshCaptainImages();
    if (NetworkDraft::Instance()->IsDraftActive())
    {
        int countdown = NetworkDraft::Instance()->GetCaptainDraftCountdown();
        if (mDraftCountdown != countdown)
        {
            mDraftCountdown = countdown;
            UpdateDraftTimer(countdown);
        }
        if (countdown == 0 && !mDraftExitDone)
        {
            int captain;
            if (mSidePads[0] != -1 && mSelectedCaptains[0] != -1
                && !NetworkDraft::Instance()->IsCaptainTaken(sCaptainButtonSelectionOrder[mSelectedCaptains[0]]))
                captain = sCaptainButtonSelectionOrder[mSelectedCaptains[0]];
            else if (mConfirmed[0] == true)
                captain = mCaptainIds[0];
            else
                captain = NetworkDraft::Instance()->GetRandomAvailableCaptain();
            GameInfoManager::Instance()->SetTeam(0, captain);
            mScenePhase = PHASE_EXITING_FORWARD;
            GetNavigationScene()->HideButtons();
            mPDALayers[0]->SetActiveSlide("out", true, false);
            mPDALayers[1]->SetActiveSlide("out", true, false);
            mDraftExitDone = true;
        }
    }
}

/**
 * Offset/Address/Size: 0x277C | 0x80224814 | size: 0x168
 */
void ChooseCaptainsSceneV2::LoadCaptainTextures()
{
    FEPresentation* presentation = mPresentation;
    for (int i = 0; i < 12; ++i)
    {
        const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(gCaptainSelectionOrder[i]));
        char selected[64];
        char disabled[64];
        nlSNPrintf(selected, sizeof(selected), "captain_%s_s", captain.mName);
        nlSNPrintf(disabled, sizeof(disabled), "captain_%s_ds", captain.mName);

        TLImageInstance* selectedImage = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            presentation, "art", "Layer", selected);
        mCaptainTextures[i][1] = selectedImage->m_pTextureResource;
        TLImageInstance* disabledImage = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            presentation, "art", "Layer", disabled);
        mCaptainTextures[i][0] = disabledImage->m_pTextureResource;
    }
}

/**
 * Offset/Address/Size: 0x28E4 | 0x8022497C | size: 0x368
 */
void ChooseCaptainsSceneV2::OnCaptainPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1)
    {
        return;
    }

    eTeamSide other = side ? HOME : AWAY;
    int captain = sCaptainButtonSelectionOrder[which];
    if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
        || (captain == 11 && !IsPeteyUnlocked()))
    {
        return;
    }
    if (mSelectedCaptains[other] == which && mConfirmed[other])
    {
        return;
    }
    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE
        && NetworkDraft::Instance()->IsCaptainTaken(sCaptainButtonSelectionOrder[which]))
    {
        return;
    }

    mConfirmed[side] = true;
    mSidePads[side] = -1;
    mCaptainIds[side] = sCaptainButtonSelectionOrder[mSelectedCaptains[side]];
    mSelectDisplays[side]->SetActiveSlide("off", true, false);
    mSelectButtons[side].SetPointerState(0, index);
    for (int i = 0; i < 4; ++i)
    {
        mCaptainButtons[which].SetPointerState(0, i);
    }
    mGreenArrows[side]->m_bVisible = false;

    int selectedCaptain = sCaptainButtonSelectionOrder[mSelectedCaptains[side]];
    FEAudio::PlayAnimAudioEvent(FECharacterSound::GetCaptainAcceptSound((eTeamID)selectedCaptain), 0, 0, 1);

    if (!mChangeTextShown[side])
    {
        UpdateSelectText(side);
    }
    mSelectButtonInstances[side]->m_bVisible = true;
}

/**
 * Offset/Address/Size: 0x2C4C | 0x80224CE4 | size: 0x4C
 */
void ChooseCaptainsSceneV2::OnCaptainPointerInside(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    if (mSelectedCaptains[side] == which)
    {
        return;
    }

    OnCaptainPointerEnter(index, context);
}

/**
 * Offset/Address/Size: 0x2C98 | 0x80224D30 | size: 0x248
 */
void ChooseCaptainsSceneV2::OnCaptainPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    eTeamSide other = side ? HOME : AWAY;
    int captain = sCaptainButtonSelectionOrder[which];

    if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
        || (captain == 11 && !IsPeteyUnlocked()))
    {
        return;
    }

    if (mSelectedCaptains[other] == which && mConfirmed[other])
    {
        return;
    }

    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE
        && NetworkDraft::Instance()->IsCaptainTaken(sCaptainButtonSelectionOrder[which]))
    {
        return;
    }

    mSelectedCaptains[side] = which;
    int selectedCaptain = sCaptainButtonSelectionOrder[which];

    if (mSceneType == ST_STRIKER_CUP)
    {
        mCaptainComponents[1].SetDisplayMode(6);
        mCaptainComponents[1].SetCaptainInfo(selectedCaptain, index, 1);
    }

    mCaptainComponents[side].SetCaptainInfo(selectedCaptain, index, 1);

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mCaptainInstances[which]->SetActiveSlide("over", true, false);
    }

    mCaptainButtons[which].SetPointerState(1, index);
    mCaptainButtons[which].PlayHoverFeedback(index);

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
 * Offset/Address/Size: 0x2EE0 | 0x80224F78 | size: 0xC8
 */
void ChooseCaptainsSceneV2::OnCaptainPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (GetSide(index) == -1)
    {
        return;
    }

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mCaptainInstances[which]->SetActiveSlide("off", true, false);
    }

    mCaptainButtons[which].SetPointerState(0, index);
}

void ChooseCaptainsSceneV2::OnSelectPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    if (mSidePads[which] != -1 || GetSide(index) != -1)
        return;
    mSidePads[which] = index;
    mConfirmed[which] = false;
    mReadyPressed[which] = false;
    mCaptainComponents[which].SetReadyPromptVisible(false);
    if (!mCaptainsShown)
    {
        mCaptainsLayer->m_bVisible = true;
        mCaptainsLayer->SetActiveSlide("in", true, false);
        mCaptainsShown = true;
        FEAudio::PlayAnimAudioEvent(0xDF52130F, 0, 0, true);
        mCaptainComponents[0].ApplyCaptainColours(mCaptainIds[0], -1);
        mCaptainComponents[1].ApplyCaptainColours(mCaptainIds[1], -1);
    }
    if (!mSideJoined[which])
    {
        mSelectDisplays[which] = mSelectButtonInstances[which];
        mSideJoined[which] = true;
        SetSelectButtonBounds(which);
        TLSlide* slide = FEFinder<TLSlide, TLAT_SLIDE>::Find(mPDALayers[which], "in");
        slide->m_time = slide->GetStartTime() + slide->GetDuration();
        mPDALayers[which]->SetActiveSlide(slide, false, true);
    }
    mSelectDisplays[which]->SetActiveSlide("down", true, false);
    mSelectButtons[which].ResetPointerStates();
    mReadyButtons[which].ResetPointerStates();
    mDoneButton.ResetPointerStates();
    if (mSideJoined[which])
        FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, true);
    else
        FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, true);
}

/**
 * Offset/Address/Size: 0x33EC | 0x80225484 | size: 0x118
 */
void ChooseCaptainsSceneV2::OnSelectPointerEnter(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mSidePads[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mSelectButtons[which].HasOtherPointerState(1, index))
    {
        mSelectDisplays[which]->SetActiveSlide("over", true, false);

        if (mSideJoined[which])
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
        }
    }

    mSelectButtons[which].SetPointerState(1, index);
    mSelectButtons[which].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x3504 | 0x8022559C | size: 0xD8
 */
void ChooseCaptainsSceneV2::OnSelectPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mSidePads[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mSelectButtons[which].HasOtherPointerState(1, index))
    {
        mSelectDisplays[which]->SetActiveSlide("off", true, false);
    }

    mSelectButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x35DC | 0x80225674 | size: 0x158
 */
void ChooseCaptainsSceneV2::OnSelectPointerInside(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mSidePads[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (mSelectButtons[which].GetPointerState(index) == 0 && mSidePads[which] == -1 && GetSide(index) == -1)
    {
        if (!mSelectButtons[which].HasOtherPointerState(1, index))
        {
            mSelectDisplays[which]->SetActiveSlide("over", true, false);

            if (mSideJoined[which])
            {
                FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
            }
        }

        mSelectButtons[which].SetPointerState(1, index);
        mSelectButtons[which].PlayHoverFeedback(index);
    }
}

/**
 * Offset/Address/Size: 0x3734 | 0x802257CC | size: 0xEC
 */
void ChooseCaptainsSceneV2::OnReadyPointerPress(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mSidePads[which] != -1 || side != -1 || !mConfirmed[which] || mReadyPressed[which])
    {
        return;
    }

    mOkButtonInstances[which]->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mReadyButtons[which].SetPointerState(0, i);
    }

    mReadyPressed[which] = true;
    mCaptainComponents[which].SetReadyPromptVisible(true);
}

/**
 * Offset/Address/Size: 0x3820 | 0x802258B8 | size: 0xF4
 */
void ChooseCaptainsSceneV2::OnReadyPointerEnter(int index, void* context)
{
    if (mSidePads[(unsigned long)context] != -1 || GetSide(index) != -1
        || !mConfirmed[(unsigned long)context] || mReadyPressed[(unsigned long)context])
    {
        return;
    }

    if (!mReadyButtons[(unsigned long)context].HasOtherPointerState(1, index))
    {
        mOkButtonInstances[(unsigned long)context]->SetActiveSlide("over", true, false);
    }

    mReadyButtons[(unsigned long)context].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3914 | 0x802259AC | size: 0xF4
 */
void ChooseCaptainsSceneV2::OnReadyPointerLeave(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mSidePads[which] != -1 || GetSide(index) != -1 || !mConfirmed[which] || mReadyPressed[which])
    {
        return;
    }

    if (!mReadyButtons[which].HasOtherPointerState(1, index))
    {
        mOkButtonInstances[which]->SetActiveSlide("off", true, false);
    }

    mReadyButtons[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3A08 | 0x80225AA0 | size: 0x138
 */
void ChooseCaptainsSceneV2::OnReadyPointerInside(int index, void* context)
{
    if (mSidePads[(unsigned long)context] != -1 || GetSide(index) != -1
        || !mConfirmed[(unsigned long)context] || mReadyPressed[(unsigned long)context])
    {
        return;
    }

    if (mReadyButtons[(unsigned long)context].GetPointerState(index) == 0)
    {
        OnReadyPointerEnter(index, context);
    }
}

/**
 * Offset/Address/Size: 0x3B40 | 0x80225BD8 | size: 0x1D0
 */
void ChooseCaptainsSceneV2::OnDonePointerPress(int index, void* context)
{
    if (!mConfirmed[0] || !mConfirmed[1])
    {
        return;
    }

    mDoneButtonInstance->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mDoneButton.SetPointerState(0, i);
    }

    mDoneButton.Disable();
    mInputSuppressed = true;

    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x2A10C1C3, 0, 0, 1);

    mScenePhase = PHASE_EXITING_FORWARD;
    GetNavigationScene()->HideButtons();

    mPDALayers[0]->SetActiveSlide("out", true, false);
    mPDALayers[1]->SetActiveSlide("out", true, false);
    mCupPDALayer->SetActiveSlide("out", true, false);

    if (mSceneType == ST_STRIKER_CUP)
    {
        g_pCupManager->mPendingCupTeam = mCaptainIds[0];
    }
    else
    {
        GameInfoManager::Instance()->SetTeam(0, mCaptainIds[0]);
        if (GameInfoManager::Instance()->mIsOnlineMode == 0)
        {
            GameInfoManager::Instance()->SetTeam(1, mCaptainIds[1]);
        }
    }
}

/**
 * Offset/Address/Size: 0x3D10 | 0x80225DA8 | size: 0xB0
 */
void ChooseCaptainsSceneV2::OnDonePointerEnter(int index, void* context)
{
    if (!mConfirmed[0] || !mConfirmed[1])
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
 * Offset/Address/Size: 0x3DC0 | 0x80225E58 | size: 0x8C
 */
void ChooseCaptainsSceneV2::OnDonePointerLeave(int index, void* context)
{
    if (!mConfirmed[0] || !mConfirmed[1])
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
 * Offset/Address/Size: 0x3E4C | 0x80225EE4 | size: 0xCC
 */
void ChooseCaptainsSceneV2::OnDonePointerInside(int index, void* context)
{
    if (!mConfirmed[0] || !mConfirmed[1])
    {
        return;
    }

    if (mDoneButton.GetPointerState(index) == 0)
    {
        OnDonePointerEnter(index, context);
    }
}

void ChooseCaptainsSceneV2::InitializeCaptainButtons()
{
    typedef Detail::MemFunImpl<void, void (ChooseCaptainsSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseCaptainsSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnCaptainPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnCaptainPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback inside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnCaptainPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnCaptainPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    TLInstance* captains = FEFinder<TLInstance, 4>::Find(mCaptainsLayer, "in", "captains");
    feVector3 scale = mCaptainsLayer->GetAssetScale();
    feVector3 position = mCaptainsLayer->GetAssetPosition();
    feVector3 captainPosition = captains->GetAssetPosition();
    for (int i = 0; i < 12; ++i)
    {
        feVector3 itemPosition = mCaptainInstances[i]->GetAssetPosition();
        float x = position.f.x + captainPosition.f.x;
        float y = position.f.y + captainPosition.f.y;
        x += itemPosition.f.x * scale.f.x;
        y += itemPosition.f.y * scale.f.y;
        mCaptainButtons[i].SetInstanceBounds(mCaptainImages[i], false, x, y, 1.0f, 1.0f);
        mCaptainButtons[i].SetPointerEnterCallback(enter);
        mCaptainButtons[i].SetPointerLeaveCallback(leave);
        mCaptainButtons[i].SetPointerInsideCallback(inside);
        mCaptainButtons[i].SetPointerPressCallback(press);
    }
}

void ChooseCaptainsSceneV2::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (ChooseCaptainsSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseCaptainsSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback selectEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnSelectPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnSelectPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnSelectPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectPress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnSelectPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnReadyPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnReadyPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnReadyPointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyPress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnReadyPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnDonePointerInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback donePress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));

    for (int side = 0; side < 2; ++side)
    {
        if ((GameInfoManager::Instance()->mIsOnlineMode || mSceneType == ST_STRIKER_CUP) && side == 1)
            continue;

        SetSelectButtonBounds(side);

        mSelectButtons[side].SetPointerEnterCallback(selectEnter);
        mSelectButtons[side].SetPointerLeaveCallback(selectLeave);
        mSelectButtons[side].SetPointerInsideCallback(selectInside);
        mSelectButtons[side].SetPointerPressCallback(selectPress);
    }

    SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, 0);
    mDoneButton.SetPointerEnterCallback(doneEnter);
    mDoneButton.SetPointerLeaveCallback(doneLeave);
    mDoneButton.SetPointerInsideCallback(doneInside);
    mDoneButton.SetPointerPressCallback(donePress);
}

void ChooseCaptainsSceneV2::UpdatePointerCursors()
{
    bool idle[2] = { true, true };
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mSidePads[0] != -1 && mSidePads[0] != i && mSidePads[1] != -1 && mSidePads[1] != i)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mSceneType == ST_STRIKER_CUP && i != gFEControllerIndex)
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
            bool overButton = mDoneButton.GetPointerState(i) == 1;
            if (!overButton)
            {
                for (int side = 0; side < 2; ++side)
                {
                    if (mReadyButtons[side].GetPointerState(i) == 1 || mSelectButtons[side].GetPointerState(i) == 1)
                    {
                        overButton = true;
                        break;
                    }
                }
            }
            if (!overButton)
            {
                for (int j = 0; j < 12; ++j)
                {
                    if (mCaptainButtons[j].GetPointerState(i) == 1)
                    {
                        if (mSidePads[0] == i)
                            idle[0] = false;
                        else if (mSidePads[1] == i)
                            idle[1] = false;
                        break;
                    }
                }
            }
        }
    }
    for (int side = 0; side < 2; ++side)
    {
        if (mSidePads[side] != -1 && idle[side] == true)
        {
            mSelectedCaptains[side] = -1;
            mCaptainComponents[side].SetCaptainInfo(-1, mSidePads[side], 1);
            if (mSceneType == ST_STRIKER_CUP)
            {
                mCaptainComponents[1].SetCaptainInfo(-1, mSidePads[side], 1);
                mCaptainComponents[1].SetDisplayMode(0);
            }
        }
    }
    for (int i = 0; i < 12; ++i)
    {
        if (!mCaptainButtons[i].HasOtherPointerState(1, -1))
            mCaptainInstances[i]->SetActiveSlide("off", true, false);
    }
}

void ChooseCaptainsSceneV2::RefreshCaptainImages()
{
    for (int i = 0; i < 12; ++i)
    {
        int captain = sCaptainButtonSelectionOrder[i];
        char name[8];
        char texture[32];
        nlSNPrintf(name, sizeof(name), "%d", i);
        nlSNPrintf(texture, sizeof(texture), "%02d_dummy_texture", i);
        TLInstance* group = FEFinder<TLInstance, -1>::Find<TLSlide>(mCaptainInstances[i]->GetActiveSlide(), name);
        mCaptainImages[i] = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(group, texture);
        TLInstance* noise = FEFinder<TLInstance, -1>::Find(group, "noise");
        noise->m_bVisible = false;
        if ((mConfirmed[0] && mSelectedCaptains[0] == i) || (mConfirmed[1] && mSelectedCaptains[1] == i))
        {
            mCaptainImages[i]->SetTextureResource(mCaptainTextures[i][0]);
        }
        else if (NetworkDraft::Instance()->IsDraftActive() && NetworkDraft::Instance()->IsCaptainTaken(captain))
        {
            mCaptainImages[i]->SetTextureResource(mCaptainTextures[i][0]);
        }
        else if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
                 || (captain == 11 && !IsPeteyUnlocked()))
        {
            noise->m_bVisible = true;
        }
        else
        {
            mCaptainImages[i]->SetTextureResource(mCaptainTextures[i][1]);
        }
    }
}

void ChooseCaptainsSceneV2::UpdateDoneButton()
{
    TLSlide* slide = mDoneButtonInstance->GetActiveSlide();
    if (mDoneButton.mDisabled && mBothConfirmed
        && slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration()
        && nlStrNCmp<char>(slide->m_szName, "in", 4) == 0)
    {
        mCaptainComponents[0].ApplyCaptainColours(mCaptainIds[0], mCaptainIds[1]);
        mCaptainComponents[1].ApplyCaptainColours(mCaptainIds[1], mCaptainIds[0]);
        mDoneButton.Enable();
    }
    if (mConfirmed[0] && mConfirmed[1] && !mBothConfirmed)
    {
        mBothConfirmed = true;
        mDoneButtonInstance->m_bVisible = true;
        mDoneButtonInstance->SetActiveSlide("in", true, false);
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, true);
    }
    else if (!mConfirmed[0] || !mConfirmed[1])
    {
        mBothConfirmed = false;
        mDoneButtonInstance->m_bVisible = false;
        mDoneButton.Disable();
    }
    if (mBothConfirmed && mCaptainsShown)
    {
        mCaptainsShown = false;
        mCaptainsLayer->SetActiveSlide("out", true, false);
        FEAudio::PlayAnimAudioEvent(0x0B8C09FA, 0, 0, true);
    }
}

/**
 * Offset/Address/Size: 0x5B34 | 0x80227BCC | size: 0x1DC
 */
void ChooseCaptainsSceneV2::UpdateSelectText(int which)
{
    TLTextInstance* offText = FEFinder<TLTextInstance, 3>::FindOrDefault(mSelectButtonInstances[which],
        "off",
        "Group",
        "select text");
    TLTextInstance* overText = FEFinder<TLTextInstance, 3>::FindOrDefault(mSelectButtonInstances[which],
        "over",
        "Group",
        "select text");
    TLTextInstance* downText = FEFinder<TLTextInstance, 3>::FindOrDefault(mSelectButtonInstances[which],
        "down",
        "Group",
        "select text");

    if (!mChangeTextShown[which])
    {
        offText->SetStringId("CHANGE_CAPTAIN");
        overText->SetStringId("CHANGE_CAPTAIN");
        downText->SetStringId("CHANGE_CAPTAIN");
        mChangeTextShown[which] = true;
    }
    else
    {
        offText->SetStringId("SELECT");
        overText->SetStringId("SELECT");
        downText->SetStringId("SELECT");
        mChangeTextShown[which] = false;
    }
}

void ChooseCaptainsSceneV2::ReleaseController(int index)
{
    eTeamSide side;
    if (mSidePads[0] == index)
        side = HOME;
    else
        side = AWAY;
    mSelectDisplays[side]->SetActiveSlide("off", true, false);
    mSelectButtons[side].SetPointerState(0, index);
    mCaptainComponents[side].SetCaptainInfo(-1, index, 1);
    mGreenArrows[side]->m_bVisible = false;
    mSidePads[side] = -1;
    mSelectDisplays[side] = mPDALayers[side];
    mPDALayers[side]->SetActiveSlide("off", true, false);
    mSelectButtonInstances[side]->m_bVisible = false;
    mSideJoined[side] = false;
    SetSelectButtonBounds(side);
    if (mSceneType == ST_STRIKER_CUP)
    {
        mCaptainComponents[1].SetCaptainInfo(-1, mSidePads[side], 1);
        mCaptainComponents[1].SetDisplayMode(0);
    }
}

/**
 * Offset/Address/Size: 0x60A8 | 0x80228140 | size: 0x5C
 */
void ChooseCaptainsSceneV2::OnDisconnectDismissed()
{
    mPopupActive = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push(SCENE_ONLINE_MENU, SCREEN_BACK, true);
}

int ChooseCaptainsSceneV2::GetSide(unsigned long pad)
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

void ChooseCaptainsSceneV2::SetSelectButtonBounds(int side)
{
    if (mSideJoined[side])
    {
        TLInstance* button = FEFinder<TLInstance, 4>::Find(mSelectButtonInstances[side], "off", "Group", "button_select");
        feVector3 position = mSelectButtonInstances[side]->GetAssetPosition();
        mSelectButtons[side].SetInstanceBounds(button, false, position.f.x - 157.0f, position.f.y + 159.0f, 0.75f, 0.5f);
    }
    else
    {
        feVector3 position = mSelectDisplays[side]->GetAssetPosition();
        TLInstance* clickable;
        if (side == 0)
            clickable = FEFinder<TLInstance, 4>::Find(mSelectDisplays[side], "off", "clickable");
        else
            clickable = FEFinder<TLInstance, 4>::Find(mSelectDisplays[side], "off", "clickable_AWAY");
        feVector3 clickablePosition = clickable->GetAssetPosition();
        TLInstance* button = FEFinder<TLInstance, 4>::Find(clickable, "attributes_frame2");
        mSelectButtons[side].SetInstanceBounds(button, true, position.f.x + clickablePosition.f.x, position.f.y + clickablePosition.f.y, 1.0f, 1.0f);
    }
}

inline void ChooseCaptainsSceneV2::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    NetworkDraft::Instance()->UnregisterMessageReceivers();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(Bind<void>(MemFun(&ChooseCaptainsSceneV2::OnDisconnectDismissed), this)));
        mPopupActive = true;
    }
}
