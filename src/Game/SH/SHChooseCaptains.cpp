#include "Game/SH/SHChooseCaptains.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
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
#include "Game/NetworkLobby.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"

static int lbl_8051CE60[12] = { 0, 5, 3, 6, 4, 7, 1, 8, 2, 9, 10, 11 };
extern int gCaptainSelectionOrder[12];

/**
 * Offset/Address/Size: 0x0 | 0x80222098 | size: 0x468
 */
ChooseCaptainsSceneV2::ChooseCaptainsSceneV2(SceneType sceneType, ScreenMovement movement)
    : mUnidentified1C(false)
    , mSceneType(sceneType)
    , mMovement(movement)
    , mUnidentified4A(false)
    , mUnidentified4B(false)
    , mUnidentified4C(false)
    , mUnidentified4D(false)
    , mUnidentified4E(false)
    , mUnidentified1314(0)
    , mUnidentified137C(false)
    , mUnidentified1380(0)
{
    mUnidentified28[0] = -1;
    mUnidentified28[1] = -1;
    mUnidentified132C[0] = 0;
    mUnidentified132C[1] = 0;

    mUnidentified920[0].mContext = (void*)0;
    mUnidentified920[0].mSpeakerEnabled = false;
    mUnidentified920[1].mContext = (void*)1;
    mUnidentified920[1].mSpeakerEnabled = false;

    mUnidentifiedBF0.mSpeakerEnabled = false;

    mUnidentifiedA88[0].mContext = (void*)0;
    mUnidentifiedA88[0].Disable();
    mUnidentifiedA88[1].mContext = (void*)1;
    mUnidentifiedA88[1].Disable();

    mUnidentified30[0] = -1;
    mUnidentified30[1] = -1;
    mUnidentified38[0] = false;
    if (GameInfoManager::Instance()->IsInMode3() || GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mUnidentified38[1] = true;
    }
    else
    {
        mUnidentified38[1] = false;
    }
    mUnidentified3A[0] = false;
    mUnidentified3A[1] = false;
    mUnidentified3C[0] = false;
    mUnidentified3C[1] = false;
    mUnidentified40[0] = -1;
    mUnidentified40[1] = -1;
    mUnidentified48[0] = movement == SCREEN_BACK;
    mUnidentified48[1] = movement == SCREEN_BACK;

    mBackButton.SetPopScene(false);
    if (mSceneType == ST_STRIKER_CUP)
    {
        mBackButton.SetPushBackScene(false);
    }

    if (movement == SCREEN_BACK)
    {
        if (mSceneType == ST_STRIKER_CUP)
        {
            int team = g_pCupManager->unknown_0x8A28;
            mUnidentified38[0] = true;
            mUnidentified40[0] = team;
        }
        else
        {
            mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
            mUnidentified38[0] = true;
            if (GameInfoManager::Instance()->mIsOnlineMode == 0)
            {
                int team = GameInfoManager::Instance()->GetTeam(1);
                mUnidentified38[1] = true;
                mUnidentified40[1] = team;
            }
        }
    }

    for (int i = 0; i < 12; ++i)
    {
        mCaptainButtons[i].mContext = (void*)i;
        mCaptainButtons[i].mSpeakerEnabled = false;

        if (mUnidentified40[0] == lbl_8051CE60[i])
        {
            mUnidentified30[0] = i;
        }
        else if (mUnidentified40[1] == lbl_8051CE60[i])
        {
            mUnidentified30[1] = i;
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
    mUnidentified1320 = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "CAPTAINS");
    mUnidentified1320->SetVisible(false);

    TLInstance* captains = FEFinder<TLInstance, -1>::Find<TLSlide>(mUnidentified1320->GetActiveSlide(), "captains");
    for (int i = 0; i < 12; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "captain%d", i);
        mUnidentified12D4[i] = (TLComponentInstance*)FEFinder<TLInstance, TLAT_COMPONENT>::Find(captains, name);
    }

    mUnidentified1324[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left");
    mUnidentified1324[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right");
    mUnidentified132C[0] = mUnidentified1324[0];
    mUnidentified132C[1] = mUnidentified1324[1];

    mUnidentified1304[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[0]->GetActiveSlide(), "select button");
    mUnidentified130C[0] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[0]->GetActiveSlide(), "button_ok");
    mUnidentified130C[0]->SetVisible(false);
    TLComponentInstance* screens = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[0]->GetActiveSlide(), "pda_screens");
    mUnidentified1318[0] = FEFinder<TLInstance, TLAT_COMPONENT>::Find(screens, "empty", "green_arrow");
    mUnidentified1318[0]->SetVisible(false);

    TLComponentInstance* back = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        back = navigation->GetButton(4);
        mUnidentified1314 = navigation->GetButton(0x20);
    }
    mUnidentified1314->SetActiveSlide("off", true, false);

    TLComponentInstance* cupPda = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right RTSC");
    mUnidentified1334 = cupPda;
    if (mSceneType != ST_STRIKER_CUP)
    {
        mUnidentified1304[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[1]->GetActiveSlide(), "select button");
        mUnidentified130C[1] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[1]->GetActiveSlide(), "button_ok");
        mUnidentified130C[1]->SetVisible(false);
        screens = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(mUnidentified1324[1]->GetActiveSlide(), "pda_screens");
        mUnidentified1318[1] = FEFinder<TLInstance, TLAT_COMPONENT>::Find(screens, "empty", "green_arrow");
        mUnidentified1318[1]->SetVisible(false);
        cupPda->SetVisible(false);
        mCaptainComponents[1].Initialize(mUnidentified1324[1], 1, 0);
    }
    else
    {
        mCaptainComponents[1].Initialize(cupPda, 1, 0);
        mUnidentified1324[1]->SetVisible(false);
        TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(cupPda->GetActiveSlide(), "scrollbar");
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(scrollbar->GetActiveSlide(), "up_arrow")->SetActiveSlide("unused", true, false);
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(scrollbar->GetActiveSlide(), "down_arrow")->SetActiveSlide("unused", true, false);
        FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault<TLSlide>(scrollbar->GetActiveSlide(), "track", "btn_scroll_minmax")->SetVisible(false);
    }

    mCaptainComponents[0].Initialize(mUnidentified1324[0], 0, 0);
    for (int side = 0; side < 2; ++side)
    {
        mCaptainComponents[side].SetDisplayMode(0);
        mCaptainComponents[side].SetReadyPromptVisible(false);
        mCaptainComponents[side].SetCaptainInfo(mUnidentified40[side], 0, 1);
    }
    mCaptainComponents[0].ShowSlideIn();
    fn_80227BCC(0);
    mUnidentified1304[0]->SetVisible(false);

    TLComponentInstance* in = (TLComponentInstance*)FEFinder<TLInstance, TLAT_COMPONENT>::Find(mUnidentified1324[0], "in", "pda_screens");
    TLInstance* attributes = FEFinder<TLInstance, TLAT_COMPONENT>::Find(in, "Slide1", "attributes_captains");
    TLComponentInstance* title = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(((TLComponentInstance*)attributes)->GetActiveSlide(), "attributes_captains", "TITLE");
    if (GameInfoManager::Instance()->IsOnline())
    {
        title->SetActiveSlide("captain", true, false);
        mCaptainComponents[1].ShowSlideIn();
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right", "OFF", "clickable_AWAY", "CHOOSE AWAY CAPTAIN")->SetVisible(false);
        mUnidentified1304[1]->SetVisible(false);
    }
    else if (mSceneType == ST_STRIKER_CUP)
    {
        title->SetActiveSlide("captain", true, false);
        mCaptainComponents[1].ShowSlideIn();
    }
    else
    {
        mCaptainComponents[1].ShowSlideIn();
        mUnidentified1304[1]->SetVisible(false);
        fn_80227BCC(1);
        FEMusic::StartStreamIfDifferent(2);
    }

    fn_80224814();
    fn_80227608();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    if (NetworkDraft::Instance()->fn_8022819C())
    {
        mUnidentified1368 = NetworkDraft::Instance()->fn_802281B0();
        fn_80223B98(mUnidentified1368);
        mBackButton.Disable();
        back->SetVisible(false);
    }
    else
    {
        fn_80223B98(-1);
        mBackButton.SetButtonInstance(back);
    }

    if (mMovement == SCREEN_BACK)
    {
        mUnidentified132C[0] = mUnidentified1304[0];
        mUnidentified1304[0]->SetVisible(true);
        if (!GameInfoManager::Instance()->IsInMode3() && !GameInfoManager::Instance()->IsOnline())
        {
            mCaptainComponents[0].ApplyCaptainColours(mUnidentified40[0], mUnidentified40[1]);
            mCaptainComponents[1].ApplyCaptainColours(mUnidentified40[1], mUnidentified40[0]);
            mUnidentified132C[1] = mUnidentified1304[1];
            mUnidentified1304[1]->SetVisible(true);
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
void ChooseCaptainsSceneV2::fn_80223B98(int value)
{
    TLSlide* slide = mPresentation->GetActiveSlide();

    FEFinder<TLTextInstance, 3>::FindOrDefault<TLSlide>(slide, "Layer", "TimerText")->m_bVisible = false;

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
 * Offset/Address/Size: 0x1C70 | 0x80223D08 | size: 0x80
 */
bool fn_80223D08(int pad)
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
    if (mUnidentified1C && !g_pFEInput->HasInputLock(this))
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
    if (mUnidentified1380 == 0 || mUnidentified1380 == 2 || mUnidentified1380 == 3)
    {
        TLSlide* leftSlide = mUnidentified1324[0]->GetActiveSlide();
        TLSlide* rightSlide = mUnidentified1324[1]->GetActiveSlide();
        if (leftSlide->GetCurrentTime() < leftSlide->GetStartTime() + leftSlide->GetDuration()
            || rightSlide->GetCurrentTime() < rightSlide->GetStartTime() + rightSlide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified1380 == 0)
        {
            if (GameInfoManager::Instance()->IsOnline())
                GetNavigationScene()->SetButtons(0x20, true);
            else
                GetNavigationScene()->SetButtons(0x24, true);
            fn_80227988();
            if (!mUnidentified48[0])
                mUnidentified1324[0]->SetActiveSlide("off", true, false);
            if (!mUnidentified48[1])
                mUnidentified1324[1]->SetActiveSlide("off", true, false);
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
            if (mMovement == SCREEN_BACK && mSceneType == ST_STRIKER_CUP)
            {
                int captain = g_pCupManager->fn_8022ED7C();
                mCaptainComponents[1].SetDisplayMode(6);
                mCaptainComponents[1].SetCaptainInfo(captain, 0, 1);
            }
            mUnidentified1380 = 1;
            fn_80226524();
            mUnidentified4B = true;
        }
        else if (mUnidentified1380 == 2)
        {
            if (mSceneType == ST_STRIKER_CUP)
                GameSceneManager::Instance()->Push((SceneList)7, SCREEN_FORWARD, true);
            else
            {
                if (GameInfoManager::Instance()->IsOnline())
                    NetworkDraft::Instance()->SendCaptainChoice();
                GameSceneManager::Instance()->Push((SceneList)3, SCREEN_FORWARD, true);
            }
            return;
        }
        else if (mUnidentified1380 == 3)
        {
            if (GameInfoManager::Instance()->IsOnline())
                GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_BACK, true);
            else if (mSceneType != ST_STRIKER_CUP)
                GameSceneManager::Instance()->Push((SceneList)0x1B, SCREEN_BACK, true);
            if (mSceneType == ST_STRIKER_CUP)
            {
                GameSceneManager::Instance()->Pop();
                g_pCupManager->fn_8010C52C(-1);
                FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
                FrontEndPresentation::GetInstance()->Call("TransitionStrikerCupToMainMenu");
            }
            else
                FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, true);
            return;
        }
    }
    if (!mUnidentified4A)
    {
        fn_80225FB0();
        mUnidentified4A = true;
        return;
    }
    if (mUnidentified4E)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (g_pNetworkSessionBase->GetSessionMode() != 0 && !fn_80223D08(i))
        {
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            continue;
        }
        if (mSceneType == ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            if (mUnidentified28[0] == i || mUnidentified28[1] == i)
                fn_80227DA8(i);
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
            mUnidentified920[side].HandlePointerEvent(&event);
        mUnidentifiedBF0.HandlePointerEvent(&event);
        if (mUnidentified4E)
            return;
        if (!NetworkDraft::Instance()->fn_8022819C())
        {
            if (mBackButton.UpdateBackButton(event, dt))
            {
                mUnidentified1380 = 3;
                GetNavigationScene()->HideButtons();
                mUnidentified1324[0]->SetActiveSlide("out", true, false);
                mUnidentified1324[1]->SetActiveSlide("out", true, false);
                mUnidentified1334->SetActiveSlide("out", true, false);
                if (mUnidentified4C)
                    mUnidentified1320->SetActiveSlide("out", true, false);
                return;
            }
        }
        if ((mUnidentified28[0] == i || mUnidentified28[1] == i) && !g_pFEInput->IsConnected((eFEINPUT_PAD)i))
            fn_80227DA8(i);
    }
    fn_80227988();
    fn_80227308();
    fn_80227608();
    if (NetworkDraft::Instance()->fn_8022819C())
    {
        int countdown = NetworkDraft::Instance()->fn_802281B0();
        if (mUnidentified1368 != countdown)
        {
            mUnidentified1368 = countdown;
            fn_80223B98(countdown);
        }
        if (countdown == 0 && !mUnidentified137C)
        {
            int captain;
            if (mUnidentified28[0] != -1 && mUnidentified30[0] != -1
                && !NetworkDraft::Instance()->IsCaptainTaken(lbl_8051CE60[mUnidentified30[0]]))
                captain = lbl_8051CE60[mUnidentified30[0]];
            else if (mUnidentified38[0] == true)
                captain = mUnidentified40[0];
            else
                captain = NetworkDraft::Instance()->GetRandomAvailableCaptain();
            GameInfoManager::Instance()->SetTeam(0, captain);
            mUnidentified1380 = 2;
            GetNavigationScene()->HideButtons();
            mUnidentified1324[0]->SetActiveSlide("out", true, false);
            mUnidentified1324[1]->SetActiveSlide("out", true, false);
            mUnidentified137C = true;
        }
    }
}

/**
 * Offset/Address/Size: 0x277C | 0x80224814 | size: 0x168
 */
void ChooseCaptainsSceneV2::fn_80224814()
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
        mUnidentified50[i][1] = selectedImage->m_pTextureResource;
        TLImageInstance* disabledImage = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            presentation, "art", "Layer", disabled);
        mUnidentified50[i][0] = disabledImage->m_pTextureResource;
    }
}

/**
 * Offset/Address/Size: 0x28E4 | 0x8022497C | size: 0x368
 */
void ChooseCaptainsSceneV2::fn_8022497C(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1)
    {
        return;
    }

    eTeamSide other = side ? HOME : AWAY;
    int captain = lbl_8051CE60[which];
    if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
        || (captain == 11 && !IsPeteyUnlocked()))
    {
        return;
    }
    if (mUnidentified30[other] == which && mUnidentified38[other])
    {
        return;
    }
    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE
        && NetworkDraft::Instance()->IsCaptainTaken(lbl_8051CE60[which]))
    {
        return;
    }

    mUnidentified38[side] = true;
    mUnidentified28[side] = -1;
    mUnidentified40[side] = lbl_8051CE60[mUnidentified30[side]];
    mUnidentified132C[side]->SetActiveSlide("off", true, false);
    mUnidentified920[side].SetPointerState(0, index);
    for (int i = 0; i < 4; ++i)
    {
        mCaptainButtons[which].SetPointerState(0, i);
    }
    mUnidentified1318[side]->m_bVisible = false;

    int selectedCaptain = lbl_8051CE60[mUnidentified30[side]];
    FEAudio::PlayAnimAudioEvent(FECharacterSound::GetCaptainAcceptSound((eTeamID)selectedCaptain), 0, 0, 1);

    if (!mUnidentified3C[side])
    {
        fn_80227BCC(side);
    }
    mUnidentified1304[side]->m_bVisible = true;
}

/**
 * Offset/Address/Size: 0x2C4C | 0x80224CE4 | size: 0x4C
 */
void ChooseCaptainsSceneV2::fn_80224CE4(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    if (mUnidentified30[side] == which)
    {
        return;
    }

    fn_80224D30(index, context);
}

/**
 * Offset/Address/Size: 0x2C98 | 0x80224D30 | size: 0x248
 */
void ChooseCaptainsSceneV2::fn_80224D30(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (side == -1)
    {
        return;
    }

    eTeamSide other = side ? HOME : AWAY;
    int captain = lbl_8051CE60[which];

    if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
        || (captain == 11 && !IsPeteyUnlocked()))
    {
        return;
    }

    if (mUnidentified30[other] == which && mUnidentified38[other])
    {
        return;
    }

    if (NetworkDraft::Instance()->mState != NET_DRAFT_IDLE
        && NetworkDraft::Instance()->IsCaptainTaken(lbl_8051CE60[which]))
    {
        return;
    }

    mUnidentified30[side] = which;
    int selectedCaptain = lbl_8051CE60[which];

    if (mSceneType == ST_STRIKER_CUP)
    {
        mCaptainComponents[1].SetDisplayMode(6);
        mCaptainComponents[1].SetCaptainInfo(selectedCaptain, index, 1);
    }

    mCaptainComponents[side].SetCaptainInfo(selectedCaptain, index, 1);

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mUnidentified12D4[which]->SetActiveSlide("over", true, false);
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
void ChooseCaptainsSceneV2::fn_80224F78(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (GetSide(index) == -1)
    {
        return;
    }

    if (!mCaptainButtons[which].HasOtherPointerState(1, index))
    {
        mUnidentified12D4[which]->SetActiveSlide("off", true, false);
    }

    mCaptainButtons[which].SetPointerState(0, index);
}

void ChooseCaptainsSceneV2::fn_80225040(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
        return;
    mUnidentified28[which] = index;
    mUnidentified38[which] = false;
    mUnidentified3A[which] = false;
    mCaptainComponents[which].SetReadyPromptVisible(false);
    if (!mUnidentified4C)
    {
        mUnidentified1320->m_bVisible = true;
        mUnidentified1320->SetActiveSlide("in", true, false);
        mUnidentified4C = true;
        FEAudio::PlayAnimAudioEvent(0xDF52130F, 0, 0, true);
        mCaptainComponents[0].ApplyCaptainColours(mUnidentified40[0], -1);
        mCaptainComponents[1].ApplyCaptainColours(mUnidentified40[1], -1);
    }
    if (!mUnidentified48[which])
    {
        mUnidentified132C[which] = mUnidentified1304[which];
        mUnidentified48[which] = true;
        SetSelectButtonBounds(which);
        TLSlide* slide = FEFinder<TLSlide, TLAT_SLIDE>::Find(mUnidentified1324[which], "in");
        slide->m_time = slide->GetStartTime() + slide->GetDuration();
        mUnidentified1324[which]->SetActiveSlide(slide, false, true);
    }
    mUnidentified132C[which]->SetActiveSlide("down", true, false);
    mUnidentified920[which].ResetPointerStates();
    mUnidentifiedA88[which].ResetPointerStates();
    mUnidentifiedBF0.ResetPointerStates();
    if (mUnidentified48[which])
        FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, true);
    else
        FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, true);
}

/**
 * Offset/Address/Size: 0x33EC | 0x80225484 | size: 0x118
 */
void ChooseCaptainsSceneV2::fn_80225484(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mUnidentified920[which].HasOtherPointerState(1, index))
    {
        mUnidentified132C[which]->SetActiveSlide("over", true, false);

        if (mUnidentified48[which])
        {
            FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
        }
        else
        {
            FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
        }
    }

    mUnidentified920[which].SetPointerState(1, index);
    mUnidentified920[which].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x3504 | 0x8022559C | size: 0xD8
 */
void ChooseCaptainsSceneV2::fn_8022559C(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (!mUnidentified920[which].HasOtherPointerState(1, index))
    {
        mUnidentified132C[which]->SetActiveSlide("off", true, false);
    }

    mUnidentified920[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x35DC | 0x80225674 | size: 0x158
 */
void ChooseCaptainsSceneV2::fn_80225674(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1)
    {
        return;
    }

    if (mUnidentified920[which].GetPointerState(index) == 0 && mUnidentified28[which] == -1 && GetSide(index) == -1)
    {
        if (!mUnidentified920[which].HasOtherPointerState(1, index))
        {
            mUnidentified132C[which]->SetActiveSlide("over", true, false);

            if (mUnidentified48[which])
            {
                FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
            }
        }

        mUnidentified920[which].SetPointerState(1, index);
        mUnidentified920[which].PlayHoverFeedback(index);
    }
}

/**
 * Offset/Address/Size: 0x3734 | 0x802257CC | size: 0xEC
 */
void ChooseCaptainsSceneV2::fn_802257CC(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified28[which] != -1 || side != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    mUnidentified130C[which]->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedA88[which].SetPointerState(0, i);
    }

    mUnidentified3A[which] = true;
    mCaptainComponents[which].SetReadyPromptVisible(true);
}

/**
 * Offset/Address/Size: 0x3820 | 0x802258B8 | size: 0xF4
 */
void ChooseCaptainsSceneV2::fn_802258B8(int index, void* context)
{
    if (mUnidentified28[(unsigned long)context] != -1 || GetSide(index) != -1
        || !mUnidentified38[(unsigned long)context] || mUnidentified3A[(unsigned long)context])
    {
        return;
    }

    if (!mUnidentifiedA88[(unsigned long)context].HasOtherPointerState(1, index))
    {
        mUnidentified130C[(unsigned long)context]->SetActiveSlide("over", true, false);
    }

    mUnidentifiedA88[(unsigned long)context].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3914 | 0x802259AC | size: 0xF4
 */
void ChooseCaptainsSceneV2::fn_802259AC(int index, void* context)
{
    unsigned long which = (unsigned long)context;

    if (mUnidentified28[which] != -1 || GetSide(index) != -1 || !mUnidentified38[which] || mUnidentified3A[which])
    {
        return;
    }

    if (!mUnidentifiedA88[which].HasOtherPointerState(1, index))
    {
        mUnidentified130C[which]->SetActiveSlide("off", true, false);
    }

    mUnidentifiedA88[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3A08 | 0x80225AA0 | size: 0x138
 */
void ChooseCaptainsSceneV2::fn_80225AA0(int index, void* context)
{
    if (mUnidentified28[(unsigned long)context] != -1 || GetSide(index) != -1
        || !mUnidentified38[(unsigned long)context] || mUnidentified3A[(unsigned long)context])
    {
        return;
    }

    if (mUnidentifiedA88[(unsigned long)context].GetPointerState(index) == 0)
    {
        fn_802258B8(index, context);
    }
}

/**
 * Offset/Address/Size: 0x3B40 | 0x80225BD8 | size: 0x1D0
 */
void ChooseCaptainsSceneV2::fn_80225BD8(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    mUnidentified1314->SetActiveSlide("down", true, false);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedBF0.SetPointerState(0, i);
    }

    mUnidentifiedBF0.Disable();
    mUnidentified4E = true;

    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x2A10C1C3, 0, 0, 1);

    mUnidentified1380 = 2;
    GetNavigationScene()->HideButtons();

    mUnidentified1324[0]->SetActiveSlide("out", true, false);
    mUnidentified1324[1]->SetActiveSlide("out", true, false);
    mUnidentified1334->SetActiveSlide("out", true, false);

    if (mSceneType == ST_STRIKER_CUP)
    {
        g_pCupManager->unknown_0x8A28 = mUnidentified40[0];
    }
    else
    {
        GameInfoManager::Instance()->SetTeam(0, mUnidentified40[0]);
        if (GameInfoManager::Instance()->mIsOnlineMode == 0)
        {
            GameInfoManager::Instance()->SetTeam(1, mUnidentified40[1]);
        }
    }
}

/**
 * Offset/Address/Size: 0x3D10 | 0x80225DA8 | size: 0xB0
 */
void ChooseCaptainsSceneV2::fn_80225DA8(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (!mUnidentifiedBF0.HasOtherPointerState(1, index))
    {
        mUnidentified1314->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mUnidentifiedBF0.SetPointerState(1, index);
    mUnidentifiedBF0.PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x3DC0 | 0x80225E58 | size: 0x8C
 */
void ChooseCaptainsSceneV2::fn_80225E58(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (!mUnidentifiedBF0.HasOtherPointerState(1, index))
    {
        mUnidentified1314->SetActiveSlide("off", true, false);
    }

    mUnidentifiedBF0.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3E4C | 0x80225EE4 | size: 0xCC
 */
void ChooseCaptainsSceneV2::fn_80225EE4(int index, void* context)
{
    if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        return;
    }

    if (mUnidentifiedBF0.GetPointerState(index) == 0)
    {
        fn_80225DA8(index, context);
    }
}

void ChooseCaptainsSceneV2::fn_80225FB0()
{
    typedef Detail::MemFunImpl<void, void (ChooseCaptainsSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseCaptainsSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80224D30), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80224F78), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback inside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80224CE4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_8022497C), this, Placeholder<0>(), Placeholder<1>()));

    TLInstance* captains = FEFinder<TLInstance, 4>::Find(mUnidentified1320, "in", "captains");
    feVector3 scale = mUnidentified1320->GetAssetScale();
    feVector3 position = mUnidentified1320->GetAssetPosition();
    feVector3 captainPosition = captains->GetAssetPosition();
    for (int i = 0; i < 12; ++i)
    {
        feVector3 itemPosition = mUnidentified12D4[i]->GetAssetPosition();
        float x = position.f.x + captainPosition.f.x;
        float y = position.f.y + captainPosition.f.y;
        x += itemPosition.f.x * scale.f.x;
        y += itemPosition.f.y * scale.f.y;
        mCaptainButtons[i].SetInstanceBounds(mUnidentified1338[i], false, x, y, 1.0f, 1.0f);
        mCaptainButtons[i].SetPointerEnterCallback(enter);
        mCaptainButtons[i].SetPointerLeaveCallback(leave);
        mCaptainButtons[i].SetPointerInsideCallback(inside);
        mCaptainButtons[i].SetPointerPressCallback(press);
    }
}

void ChooseCaptainsSceneV2::fn_80226524()
{
    typedef Detail::MemFunImpl<void, void (ChooseCaptainsSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseCaptainsSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback selectEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225484), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_8022559C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225674), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectPress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225040), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_802258B8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_802259AC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225AA0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback readyPress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_802257CC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneEnter(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225DA8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneLeave(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225E58), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneInside(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225EE4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback donePress(PointerBinding(MemFun(&ChooseCaptainsSceneV2::fn_80225BD8), this, Placeholder<0>(), Placeholder<1>()));

    for (int side = 0; side < 2; ++side)
    {
        if ((GameInfoManager::Instance()->mIsOnlineMode || mSceneType == ST_STRIKER_CUP) && side == 1)
            continue;

        SetSelectButtonBounds(side);

        mUnidentified920[side].SetPointerEnterCallback(selectEnter);
        mUnidentified920[side].SetPointerLeaveCallback(selectLeave);
        mUnidentified920[side].SetPointerInsideCallback(selectInside);
        mUnidentified920[side].SetPointerPressCallback(selectPress);
    }

    SetDoneButtonBounds(&mUnidentifiedBF0, mUnidentified1314, 0);
    mUnidentifiedBF0.SetPointerEnterCallback(doneEnter);
    mUnidentifiedBF0.SetPointerLeaveCallback(doneLeave);
    mUnidentifiedBF0.SetPointerInsideCallback(doneInside);
    mUnidentifiedBF0.SetPointerPressCallback(donePress);
}

void ChooseCaptainsSceneV2::fn_80227308()
{
    bool idle[2] = { true, true };
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mUnidentified28[0] != -1 && mUnidentified28[0] != i && mUnidentified28[1] != -1 && mUnidentified28[1] != i)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mSceneType == ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (g_pNetworkSessionBase->GetSessionMode() != 0 && !fn_80223D08(i))
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else
        {
            pointer->SetActiveSlide("cursor", true, false);
            bool overButton = mUnidentifiedBF0.GetPointerState(i) == 1;
            if (!overButton)
            {
                for (int side = 0; side < 2; ++side)
                {
                    if (mUnidentifiedA88[side].GetPointerState(i) == 1 || mUnidentified920[side].GetPointerState(i) == 1)
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
                        if (mUnidentified28[0] == i)
                            idle[0] = false;
                        else if (mUnidentified28[1] == i)
                            idle[1] = false;
                        break;
                    }
                }
            }
        }
    }
    for (int side = 0; side < 2; ++side)
    {
        if (mUnidentified28[side] != -1 && idle[side] == true)
        {
            mUnidentified30[side] = -1;
            mCaptainComponents[side].SetCaptainInfo(-1, mUnidentified28[side], 1);
            if (mSceneType == ST_STRIKER_CUP)
            {
                mCaptainComponents[1].SetCaptainInfo(-1, mUnidentified28[side], 1);
                mCaptainComponents[1].SetDisplayMode(0);
            }
        }
    }
    for (int i = 0; i < 12; ++i)
    {
        if (!mCaptainButtons[i].HasOtherPointerState(1, -1))
            mUnidentified12D4[i]->SetActiveSlide("off", true, false);
    }
}

void ChooseCaptainsSceneV2::fn_80227608()
{
    for (int i = 0; i < 12; ++i)
    {
        int captain = lbl_8051CE60[i];
        char name[8];
        char texture[32];
        nlSNPrintf(name, sizeof(name), "%d", i);
        nlSNPrintf(texture, sizeof(texture), "%02d_dummy_texture", i);
        TLInstance* group = FEFinder<TLInstance, -1>::Find<TLSlide>(mUnidentified12D4[i]->GetActiveSlide(), name);
        mUnidentified1338[i] = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(group, texture);
        TLInstance* noise = FEFinder<TLInstance, -1>::Find(group, "noise");
        noise->m_bVisible = false;
        if ((mUnidentified38[0] && mUnidentified30[0] == i) || (mUnidentified38[1] && mUnidentified30[1] == i))
        {
            mUnidentified1338[i]->SetTextureResource(mUnidentified50[i][0]);
        }
        else if (NetworkDraft::Instance()->fn_8022819C() && NetworkDraft::Instance()->IsCaptainTaken(captain))
        {
            mUnidentified1338[i]->SetTextureResource(mUnidentified50[i][0]);
        }
        else if ((captain == 9 && !IsBowserJrUnlocked()) || (captain == 10 && !IsDiddyKongUnlocked())
                 || (captain == 11 && !IsPeteyUnlocked()))
        {
            noise->m_bVisible = true;
        }
        else
        {
            mUnidentified1338[i]->SetTextureResource(mUnidentified50[i][1]);
        }
    }
}

void ChooseCaptainsSceneV2::fn_80227988()
{
    TLSlide* slide = mUnidentified1314->GetActiveSlide();
    if (mUnidentifiedBF0.mDisabled && mUnidentified4D
        && slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration()
        && nlStrNCmp<char>(slide->m_szName, "in", 4) == 0)
    {
        mCaptainComponents[0].ApplyCaptainColours(mUnidentified40[0], mUnidentified40[1]);
        mCaptainComponents[1].ApplyCaptainColours(mUnidentified40[1], mUnidentified40[0]);
        mUnidentifiedBF0.Enable();
    }
    if (mUnidentified38[0] && mUnidentified38[1] && !mUnidentified4D)
    {
        mUnidentified4D = true;
        mUnidentified1314->m_bVisible = true;
        mUnidentified1314->SetActiveSlide("in", true, false);
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, true);
    }
    else if (!mUnidentified38[0] || !mUnidentified38[1])
    {
        mUnidentified4D = false;
        mUnidentified1314->m_bVisible = false;
        mUnidentifiedBF0.Disable();
    }
    if (mUnidentified4D && mUnidentified4C)
    {
        mUnidentified4C = false;
        mUnidentified1320->SetActiveSlide("out", true, false);
        FEAudio::PlayAnimAudioEvent(0x0B8C09FA, 0, 0, true);
    }
}

/**
 * Offset/Address/Size: 0x5B34 | 0x80227BCC | size: 0x1DC
 */
void ChooseCaptainsSceneV2::fn_80227BCC(int which)
{
    TLTextInstance* offText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        "off",
        "Group",
        "select text");
    TLTextInstance* overText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        "over",
        "Group",
        "select text");
    TLTextInstance* downText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified1304[which],
        "down",
        "Group",
        "select text");

    if (!mUnidentified3C[which])
    {
        offText->SetStringId("CHANGE_CAPTAIN");
        overText->SetStringId("CHANGE_CAPTAIN");
        downText->SetStringId("CHANGE_CAPTAIN");
        mUnidentified3C[which] = true;
    }
    else
    {
        offText->SetStringId("SELECT");
        overText->SetStringId("SELECT");
        downText->SetStringId("SELECT");
        mUnidentified3C[which] = false;
    }
}

void ChooseCaptainsSceneV2::fn_80227DA8(int index)
{
    eTeamSide side;
    if (mUnidentified28[0] == index)
        side = HOME;
    else
        side = AWAY;
    mUnidentified132C[side]->SetActiveSlide("off", true, false);
    mUnidentified920[side].SetPointerState(0, index);
    mCaptainComponents[side].SetCaptainInfo(-1, index, 1);
    mUnidentified1318[side]->m_bVisible = false;
    mUnidentified28[side] = -1;
    mUnidentified132C[side] = mUnidentified1324[side];
    mUnidentified1324[side]->SetActiveSlide("off", true, false);
    mUnidentified1304[side]->m_bVisible = false;
    mUnidentified48[side] = false;
    SetSelectButtonBounds(side);
    if (mSceneType == ST_STRIKER_CUP)
    {
        mCaptainComponents[1].SetCaptainInfo(-1, mUnidentified28[side], 1);
        mCaptainComponents[1].SetDisplayMode(0);
    }
}

/**
 * Offset/Address/Size: 0x60A8 | 0x80228140 | size: 0x5C
 */
void ChooseCaptainsSceneV2::fn_80228140()
{
    mUnidentified1C = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_BACK, true);
}

int ChooseCaptainsSceneV2::GetSide(unsigned long pad)
{
    if (mUnidentified28[0] == pad)
    {
        return 0;
    }

    if (mUnidentified28[1] == pad)
    {
        return 1;
    }

    return -1;
}

void ChooseCaptainsSceneV2::SetSelectButtonBounds(int side)
{
    if (mUnidentified48[side])
    {
        TLInstance* button = FEFinder<TLInstance, 4>::Find(mUnidentified1304[side], "off", "Group", "button_select");
        feVector3 position = mUnidentified1304[side]->GetAssetPosition();
        mUnidentified920[side].SetInstanceBounds(button, false, position.f.x - 157.0f, position.f.y + 159.0f, 0.75f, 0.5f);
    }
    else
    {
        feVector3 position = mUnidentified132C[side]->GetAssetPosition();
        TLInstance* clickable;
        if (side == 0)
            clickable = FEFinder<TLInstance, 4>::Find(mUnidentified132C[side], "off", "clickable");
        else
            clickable = FEFinder<TLInstance, 4>::Find(mUnidentified132C[side], "off", "clickable_AWAY");
        feVector3 clickablePosition = clickable->GetAssetPosition();
        TLInstance* button = FEFinder<TLInstance, 4>::Find(clickable, "attributes_frame2");
        mUnidentified920[side].SetInstanceBounds(button, true, position.f.x + clickablePosition.f.x, position.f.y + clickablePosition.f.y, 1.0f, 1.0f);
    }
}

inline void ChooseCaptainsSceneV2::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    NetworkDraft::Instance()->UnregisterMessageReceivers();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(Bind<void>(MemFun(&ChooseCaptainsSceneV2::fn_80228140), this)));
        mUnidentified1C = true;
    }
}
