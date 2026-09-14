#include "Game/SH/SHChooseSidekicks.h"
#include "Game/BaseSceneHandler.inl"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feInlineHasher.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
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
#include "Game/Render/Presentation.h"
#include "NL/nlString.h"

static int lbl_8051D198[8] = { 1, 0, 5, 4, 3, 2, 6, 7 };
static const nlVector2 lbl_804E8530[2] = { { -159.0f, 187.0f }, { 151.0f, 187.0f } };

static void fn_8022E0C0();
static void fn_8022E18C();

/**
 * Offset/Address/Size: 0x0 | 0x802284B8 | size: 0x3F0
 */
ChooseSidekicksSceneV2::ChooseSidekicksSceneV2(ChooseCaptainsSceneV2::SceneType sceneType, ScreenMovement movement)
    : mUnidentified1C(false)
    , mMovement(movement)
    , mSceneType(sceneType)
    , mUnidentified4A(false)
    , mUnidentified4B(false)
    , mUnidentified4C(false)
    , mUnidentified4D(false)
    , mUnidentified1918(0)
    , mUnidentified1950(false)
    , mUnidentified1954(0)
    , mUnidentified19F8(false)
{
    int i;

    mUnidentified20[0] = -1;
    mUnidentified20[1] = -1;

    for (i = 0; i < 8; ++i)
    {
        mUnidentifiedE8[i].mContext = (void*)i;
        mUnidentifiedE8[i].mSpeakerEnabled = false;
    }

    for (int side = 0; side < 2; ++side)
    {
        for (i = 0; i < 3; ++i)
        {
            mUnidentified688[side][i].mContext = (void*)(side * 3 + i);
            mUnidentified688[side][i].mSpeakerEnabled = false;
            mUnidentifiedAC0[side][i].mContext = (void*)(side * 3 + i);
            mUnidentifiedAC0[side][i].mSpeakerEnabled = false;
            mUnidentifiedA0[side][i] = false;
        }
    }

    mUnidentifiedEF8[0].mContext = (void*)0;
    mUnidentifiedEF8[1].mContext = (void*)1;
    mUnidentifiedEF8[0].mSpeakerEnabled = false;
    mUnidentifiedEF8[1].mSpeakerEnabled = false;

    mUnidentified1060[0].mContext = (void*)0;
    mUnidentified1060[1].mContext = (void*)1;
    mUnidentified1060[0].mSpeakerEnabled = false;
    mUnidentified1060[1].mSpeakerEnabled = false;

    mUnidentified28[0] = -1;
    mUnidentified28[1] = -1;
    mUnidentified30[0] = -1;
    mUnidentified30[1] = -1;
    mUnidentified48[0] = false;
    mUnidentified48[1] = false;
    mUnidentified11C8.mSpeakerEnabled = false;

    mBackButton.SetPopScene(false);

    if (GameInfoManager::Instance()->IsInMode3())
    {
        mUnidentified40[0] = g_pCupManager->unknown_0x8A28;
        mUnidentified40[1] = -1;
    }
    else if (GameInfoManager::Instance()->mIsOnlineMode != 0)
    {
        mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
        mUnidentified40[1] = -1;
    }
    else
    {
        mUnidentified40[0] = GameInfoManager::Instance()->GetTeam(0);
        mUnidentified40[1] = GameInfoManager::Instance()->GetTeam(1);
    }

    for (int side = 0; side < 2; ++side)
    {
        for (i = 0; i < 8; ++i)
        {
            mUnidentified1958[side][i] = 0;
            mUnidentified1998[side][i] = 0;
            mUnidentified19D8[side][i] = true;
            mUnidentified19E8[side][i] = true;
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
            if (mUnidentified1958[side][i] != 0)
            {
                delete mUnidentified1958[side][i];
                mUnidentified1958[side][i] = 0;
            }

            if (mUnidentified1998[side][i] != 0)
            {
                delete mUnidentified1998[side][i];
                mUnidentified1998[side][i] = 0;
            }
        }
    }
}

int ChooseSidekicksSceneV2::GetSide(unsigned long pad)
{
    if (mUnidentified20[0] == pad)
    {
        return 0;
    }

    if (mUnidentified20[1] == pad)
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
        Presentation::GetInstance()->Call("StartStrikerCupCaptainHologramSequence");
        mUnidentified1914 = cupSidekicks;
    }
    else if (GameInfoManager::Instance()->IsOnline())
        mUnidentified1914 = cupSidekicks;
    else
        mUnidentified1914 = sidekicks;

    TLInstance* sidekick = FEFinder<TLInstance, -1>::Find<TLSlide>(mUnidentified1914->GetActiveSlide(), "SIDEKICK");
    TLComponentInstance* pda[2];
    pda[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA left");
    pda[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "PDA right");
    for (int i = 0; i < 8; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "SIDEKICK%d", i);
        mUnidentified18AC[i] = (TLComponentInstance*)FEFinder<TLInstance, 4>::Find(sidekick, name);
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
            mUnidentified18CC[side][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", texture);
            mUnidentified18E4[side][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "field_positions", "idle", "dummies", click);
            mUnidentified190C[side] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "green_arrow");
            mUnidentified190C[side]->SetVisible(false);
        }
        if (side == 0 && (GameInfoManager::Instance()->IsOnline() || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP))
        {
            FEFinder<TLComponentInstance, 4>::Find<TLSlide>(positions->GetActiveSlide(), "positions", "TITLE")->SetActiveSlide("team", true, false);
        }
    }
    mUnidentified1904[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "select button");
    mUnidentified18FC[0] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "random button");
    mUnidentified50[0].fn_801DA198(pda[0], 0);
    mUnidentified1904[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "select button");
    mUnidentified18FC[1] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "random button");
    mUnidentified50[1].fn_801DA198(pda[1], 1);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[0]->GetActiveSlide(), "button_ok")->SetVisible(false);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(pda[1]->GetActiveSlide(), "button_ok")->SetVisible(false);
    FEFinder<TLComponentInstance, 4>::Find<TLSlide>(GetPresentation()->GetActiveSlide(), "Layer", "SCREEN_TITLES")->SetActiveSlide("SIDEKICKS", true, false);

    TLComponentInstance* back = 0;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        back = navigation->GetButton(4);
        mUnidentified1918 = navigation->GetButton(0x20);
    }
    mUnidentified50[0].fn_801DCCEC();
    mUnidentified50[1].fn_801DCCEC();
    for (int side = 0; side < 2; ++side)
    {
        mCaptainComponents[side].fn_801DCFD8(pda[side], side, 0);
        mCaptainComponents[side].fn_801E0280(1);
        mCaptainComponents[side].fn_801E0B20(false);
        mCaptainComponents[side].fn_801DF85C(-1, 0, 0);
    }
    int captain;
    if (GameInfoManager::Instance()->IsInMode3())
        captain = CupManager::Instance()->fn_8022ED7C();
    else
        captain = GameInfoManager::Instance()->GetTeam(0);
    mUnidentified50[0].fn_801DA88C();
    mUnidentified50[0].fn_801DCD84(captain);
    mCaptainComponents[0].fn_801E0AD0();
    if (GameInfoManager::Instance()->IsOnline() || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
    {
        mCaptainComponents[1].fn_801DE570(false);
        mCaptainComponents[1].fn_801E0280(5);
        fn_8022E258(captain, -1);
    }
    else
    {
        mUnidentified50[1].fn_801DA88C();
        mUnidentified50[1].fn_801DCD84(GameInfoManager::Instance()->GetTeam(1));
        mCaptainComponents[1].fn_801E0AD0();
        fn_8022E258(captain, GameInfoManager::Instance()->GetTeam(1));
        FEMusic::StartStreamIfDifferent(2);
    }
    fn_8022D8A4();
    fn_8022DA2C();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    if (NetworkDraft::Instance()->fn_8022819C())
    {
        mUnidentified193C = NetworkDraft::Instance()->fn_8022ED88();
        fn_802299C4(mUnidentified193C);
        mBackButton.Disable();
        back->SetVisible(false);
    }
    else
    {
        fn_802299C4(-1);
        mBackButton.SetButtonInstance(back);
    }
}

/**
 * Offset/Address/Size: 0x150C | 0x802299C4 | size: 0x160
 */
void ChooseSidekicksSceneV2::fn_802299C4(int value)
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
    if (!fn_8022EBB8())
    {
        fn_8022EACC();
        return;
    }
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
    mCaptainComponents[0].fn_801DE584(dt);
    mCaptainComponents[1].fn_801DE584(dt);
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
        UpdateCharacterIdleAnimations(dt);
    if (mUnidentified1954 == 0 || mUnidentified1954 == 2 || mUnidentified1954 == 3)
    {
        TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
        TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
        TLSlide* leftSlide = left->GetActiveSlide();
        TLSlide* rightSlide = right->GetActiveSlide();
        if (mUnidentified1954 == 0 && !mUnidentified4A)
        {
            mUnidentified50[0].fn_801DB69C(dt);
            mUnidentified50[1].fn_801DB69C(dt);
        }
        if (leftSlide->GetCurrentTime() < leftSlide->GetStartTime() + leftSlide->GetDuration()
            || rightSlide->GetCurrentTime() < rightSlide->GetStartTime() + rightSlide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified1954 == 0)
        {
            if (!mUnidentified4A)
            {
                fn_8022DDB0();
                if (GameInfoManager::Instance()->IsOnline())
                    GetNavigationScene()->SetButtons(0x20, true);
                else
                    GetNavigationScene()->SetButtons(0x24, true);
                for (int i = 0; i < 4; ++i)
                    GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
                fn_8022C720();
                mUnidentified4A = true;
                mUnidentified1954 = 1;
            }
        }
        else if (mUnidentified1954 == 2)
        {
            fn_8022DC68();
            return;
        }
        else if (mUnidentified1954 == 3)
        {
            if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
            {
                Presentation::GetInstance()->Call("RemoveStrikerCupCaptainHologram");
                GameSceneManager::Instance()->Push((SceneList)6, SCREEN_BACK, true);
            }
            else
                GameSceneManager::Instance()->Push((SceneList)2, SCREEN_BACK, true);
            return;
        }
    }
    if (!mUnidentified4B)
    {
        fn_8022BFEC();
        mUnidentified4B = true;
    }
    if (mUnidentified4D)
        return;
    for (int i = 0; i < 4; ++i)
    {
        if (g_pNetworkSessionBase->GetSessionMode() != 0 && !fn_80223D08(i))
        {
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            continue;
        }
        if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP && i != gFEControllerIndex)
        {
            if (mUnidentified20[0] == i || mUnidentified20[1] == i)
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
            mUnidentifiedE8[button].HandlePointerEvent(&event);
        for (int side = 0; side < 2; ++side)
        {
            mUnidentifiedEF8[side].HandlePointerEvent(&event);
            mUnidentified1060[side].HandlePointerEvent(&event);
            for (int slot = 0; slot < 3; ++slot)
            {
                mUnidentified688[side][slot].HandlePointerEvent(&event);
                mUnidentifiedAC0[side][slot].HandlePointerEvent(&event);
            }
        }
        mUnidentified11C8.HandlePointerEvent(&event);
        if (mUnidentified4D)
            return;
        if (!NetworkDraft::Instance()->fn_8022819C())
        {
            if (mBackButton.UpdateBackButton(event, dt))
            {
                mUnidentified1954 = 3;
                GetNavigationScene()->HideButtons();
                TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
                TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
                left->SetActiveSlide("out", true, false);
                right->SetActiveSlide("out", true, false);
                if (mUnidentified4C)
                    mUnidentified1914->SetActiveSlide("out", true, false);
                FEAudio::PlayAnimAudioEvent(0x9478C856, 0, 0, true);
                return;
            }
        }
        if ((mUnidentified20[0] == i || mUnidentified20[1] == i) && !g_pFEInput->IsConnected((eFEINPUT_PAD)i))
        {
            ReleaseController(i);
        }
    }
    mUnidentified50[0].fn_801DB69C(dt);
    mUnidentified50[1].fn_801DB69C(dt);
    fn_8022DDB0();
    fn_8022D5F8();
    fn_8022DA2C();
    if (NetworkDraft::Instance()->fn_8022819C())
    {
        int countdown = NetworkDraft::Instance()->fn_8022ED88();
        if (mUnidentified193C != countdown)
        {
            mUnidentified193C = countdown;
            fn_802299C4(countdown);
        }
        if (countdown == 0 && !mUnidentified1950)
        {
            fn_8022DB70();
            mUnidentified1954 = 2;
            GetNavigationScene()->HideButtons();
            TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA left");
            TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "PDA right");
            left->SetActiveSlide("out", true, false);
            right->SetActiveSlide("out", true, false);
            if (mUnidentified4C)
                mUnidentified1914->SetActiveSlide("out", true, false);
            mUnidentified1950 = true;
        }
    }
}
/**
 * Offset/Address/Size: 0x26B0 | 0x8022AB68 | size: 0x184
 */
void ChooseSidekicksSceneV2::fn_8022AB68(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mUnidentified30[side] == -1)
    {
        return;
    }

    int slot = mUnidentified30[side];
    mUnidentified50[side].fn_801DCD8C(slot, lbl_8051D198[which]);
    mUnidentified18CC[side][slot]->SetActiveSlide("off", true, false);

    for (int i = 0; i < 3; ++i)
    {
        mUnidentified688[side][i].ResetPointerStates();
    }
    mUnidentifiedE8[which].SetPointerState(0, index);
    mCaptainComponents[side].fn_801E0280(1);
    mUnidentified190C[side]->m_bVisible = false;
    mUnidentified50[side].fn_801DABAC(mUnidentified30[side], 1);
    mUnidentified30[side] = -1;
    mUnidentified20[side] = -1;
    FEAudio::PlayAnimAudioEvent(FECharacterSound::GetSidekickAcceptSound((eSidekickID)lbl_8051D198[which]), 0, 0, 1);
    GameInfoManager::Instance()->mRulesTable[GetTeam(side)].mValues[slot] = (eSidekickID)lbl_8051D198[which];
}

/**
 * Offset/Address/Size: 0x2834 | 0x8022ACEC | size: 0x1B8
 */
void ChooseSidekicksSceneV2::fn_8022ACEC(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mUnidentified30[side] == -1)
    {
        return;
    }

    mUnidentified28[side] = which;
    int sidekick = lbl_8051D198[which];
    mCaptainComponents[side].fn_801E0280(3);
    mCaptainComponents[side].fn_801DF85C(sidekick, index, 0);
    mUnidentified190C[side]->m_bVisible = false;
    if (!mUnidentifiedE8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18AC[which]->SetActiveSlide("over", true, false);
    }
    mUnidentifiedE8[which].SetPointerState(1, index);
    mUnidentifiedE8[which].PlayHoverFeedback(index);

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
void ChooseSidekicksSceneV2::fn_8022AEA4(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);
    if (side == -1 || mUnidentified30[side] == -1)
    {
        return;
    }

    if (!mUnidentifiedE8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18AC[which]->SetActiveSlide("off", true, false);
    }
    mUnidentifiedE8[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x2ACC | 0x8022AF84 | size: 0x2B0
 */
void ChooseSidekicksSceneV2::fn_8022AF84(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);
    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    mUnidentified48[group] = false;
    mCaptainComponents[group].fn_801E0B20(false);
    if (!mUnidentified4C)
    {
        mUnidentified1914->m_bVisible = true;
        mUnidentified1914->SetActiveSlide("in", true, false);
        FEAudio::PlayAnimAudioEvent(0xDF52130F, 0, 0, 1);
        mUnidentified4C = true;
    }

    for (int i = 0; i < 3; ++i)
    {
        if (i == slot)
        {
            mUnidentified18CC[group][i]->SetActiveSlide("down", true, false);
            mUnidentified18E4[group][i]->SetActiveSlide("down", true, false);
        }
        else
        {
            mUnidentified18CC[group][i]->SetActiveSlide("off", true, false);
            mUnidentified18E4[group][i]->SetActiveSlide("off", true, false);
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 4; ++j)
        {
            mUnidentified688[group][i].SetPointerState(2, j);
        }
        for (int j = 0; j < 4; ++j)
        {
            mUnidentifiedAC0[group][i].SetPointerState(2, j);
        }
    }
    mUnidentified18FC[group]->SetActiveSlide("off", true, false);
    mUnidentified1904[group]->SetActiveSlide("off", true, false);
    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedEF8[group].SetPointerState(0, i);
    }
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified1060[group].SetPointerState(0, i);
    }
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified11C8.SetPointerState(0, i);
    }
    mUnidentified190C[group]->m_bVisible = true;
    mUnidentified20[group] = index;
    mUnidentified30[group] = slot;
    mUnidentifiedA0[group][slot] = false;
    mUnidentified50[group].fn_801DABAC(slot, 0);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
}

/**
 * Offset/Address/Size: 0x2D7C | 0x8022B234 | size: 0x1CC
 */
void ChooseSidekicksSceneV2::fn_8022B234(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified688[group][slot].HasOtherPointerState(1, index))
    {
        if (mUnidentifiedA0[group][slot])
        {
            mUnidentified18E4[group][slot]->SetActiveSlide("over", true, false);
        }
        else
        {
            mUnidentified18CC[group][slot]->SetActiveSlide("over", true, false);
            FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified18CC[group][slot],
                nlStringLowerHash("over"), nlStringLowerHash("CHANGE"), 0, 0, 0, 0)
                ->SetActiveSlide("Slide1", true, false);
        }

        FEAudio::PlayAnimAudioEvent(0x23628A1D, 0, 0, 1);
    }

    mUnidentified688[group][slot].SetPointerState(1, index);
    mUnidentifiedAC0[group][slot].SetPointerState(1, index);
    mUnidentified688[group][slot].PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x2F48 | 0x8022B400 | size: 0x94
 */
void ChooseSidekicksSceneV2::fn_8022B400(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    mUnidentified688[group][slot].SetPointerState(0, index);
    mUnidentifiedAC0[group][slot].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x2FDC | 0x8022B494 | size: 0x98
 */
void ChooseSidekicksSceneV2::fn_8022B494(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    bool group = which >= 3;
    int slot = !group ? which : which - 3;
    int side = GetSide(index);

    if (mUnidentified20[group] != -1 || side != -1)
    {
        return;
    }

    if (mUnidentified688[group][slot].GetPointerState(index) != 0)
    {
        return;
    }

    fn_8022B234(index, context);
}

/**
 * Offset/Address/Size: 0x3074 | 0x8022B52C | size: 0xD4
 */
void ChooseSidekicksSceneV2::fn_8022B52C(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    mUnidentified18FC[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedEF8[which].SetPointerState(2, i);
    }

    mUnidentified50[which].fn_801DCB28();
}

/**
 * Offset/Address/Size: 0x3148 | 0x8022B600 | size: 0xE8
 */
void ChooseSidekicksSceneV2::fn_8022B600(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentifiedEF8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18FC[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mUnidentifiedEF8[which].PlayHoverFeedback(index);
    mUnidentifiedEF8[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x3230 | 0x8022B6E8 | size: 0xD8
 */
void ChooseSidekicksSceneV2::fn_8022B6E8(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentifiedEF8[which].HasOtherPointerState(1, index))
    {
        mUnidentified18FC[which]->SetActiveSlide("off", true, false);
    }

    mUnidentifiedEF8[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3308 | 0x8022B7C0 | size: 0xF4
 */
void ChooseSidekicksSceneV2::fn_8022B7C0(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    mUnidentified1904[which]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified1060[which].SetPointerState(2, i);
    }

    mCaptainComponents[which].fn_801E0B20(false);
    mUnidentified48[which] = false;
    mUnidentified50[which].fn_801DCC28();
}

/**
 * Offset/Address/Size: 0x33FC | 0x8022B8B4 | size: 0xE8
 */
void ChooseSidekicksSceneV2::fn_8022B8B4(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified1060[which].HasOtherPointerState(1, index))
    {
        mUnidentified1904[which]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF35, 0, 0, 1);
    }

    mUnidentified1060[which].PlayHoverFeedback(index);
    mUnidentified1060[which].SetPointerState(1, index);
}

/**
 * Offset/Address/Size: 0x34E4 | 0x8022B99C | size: 0xD8
 */
void ChooseSidekicksSceneV2::fn_8022B99C(int index, void* context)
{
    unsigned long which = (unsigned long)context;
    int side = GetSide(index);

    if (mUnidentified20[which] != -1 || side != -1)
    {
        return;
    }

    if (!mUnidentified1060[which].HasOtherPointerState(1, index))
    {
        mUnidentified1904[which]->SetActiveSlide("off", true, false);
    }

    mUnidentified1060[which].SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x35BC | 0x8022BA74 | size: 0x370
 */
void ChooseSidekicksSceneV2::fn_8022BA74(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    mUnidentified1918->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified11C8.SetPointerState(0, i);
    }
    mUnidentified11C8.Disable();
    mUnidentified4D = true;
    mUnidentified1954 = 2;
    GetNavigationScene()->HideButtons();
    TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide,
        "Layer", "PDA left");
    TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(mPresentation->m_currentSlide,
        "Layer", "PDA right");
    left->SetActiveSlide("out", true, false);
    right->SetActiveSlide("out", true, false);
    fn_8022DB70();
    if (mUnidentified4C)
    {
        mUnidentified1914->SetActiveSlide("out", true, false);
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
void ChooseSidekicksSceneV2::fn_8022BDE4(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (!mUnidentified11C8.HasOtherPointerState(1, index))
    {
        mUnidentified1918->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mUnidentified11C8.SetPointerState(1, index);
    mUnidentified11C8.PlayHoverFeedback(index);
}

/**
 * Offset/Address/Size: 0x39DC | 0x8022BE94 | size: 0x8C
 */
void ChooseSidekicksSceneV2::fn_8022BE94(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (!mUnidentified11C8.HasOtherPointerState(1, index))
    {
        mUnidentified1918->SetActiveSlide("off", true, false);
    }

    mUnidentified11C8.SetPointerState(0, index);
}

/**
 * Offset/Address/Size: 0x3A68 | 0x8022BF20 | size: 0xCC
 */
void ChooseSidekicksSceneV2::fn_8022BF20(int index, void* context)
{
    if (mUnidentified20[0] != -1 || mUnidentified20[1] != -1)
    {
        return;
    }

    if (mUnidentified11C8.GetPointerState(index) == 0)
    {
        fn_8022BDE4(index, context);
    }
}

/**
 * Offset/Address/Size: 0x3B34 | 0x8022BFEC | size: 0x734
 */
void ChooseSidekicksSceneV2::fn_8022BFEC()
{
    typedef Detail::MemFunImpl<void, void (ChooseSidekicksSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseSidekicksSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022ACEC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022AEA4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022AB68), this, Placeholder<0>(), Placeholder<1>()));
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
        TLInstance* layer = FEFinder<TLInstance, 1>::Find(mUnidentified18AC[i], "off", name);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(layer, imageName);
        feVector3 itemPosition = mUnidentified18AC[i]->GetAssetPosition();
        float x = position.f.x + sidekickPosition.f.x;
        float y = position.f.y + sidekickPosition.f.y;
        x += itemPosition.f.x * scale.f.x;
        y += itemPosition.f.y * scale.f.y;
        mUnidentifiedE8[i].SetInstanceBounds(image, false, x, y, 1.0f, 1.0f);
        mUnidentifiedE8[i].SetPointerEnterCallback(enter);
        mUnidentifiedE8[i].SetPointerLeaveCallback(leave);
        mUnidentifiedE8[i].SetPointerPressCallback(press);
    }
}

/**
 * Offset/Address/Size: 0x4268 | 0x8022C720 | size: 0xED8
 */
void ChooseSidekicksSceneV2::fn_8022C720()
{
    typedef Detail::MemFunImpl<void, void (ChooseSidekicksSceneV2::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChooseSidekicksSceneV2*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback selectEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B600), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B6E8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback selectPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B52C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B8B4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B99C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback randomPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B7C0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022BDE4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022BE94), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback doneInside(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022BF20), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback donePress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022BA74), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotEnter(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B234), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotLeave(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B400), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotInside(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022B494), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback slotPress(PointerBinding(MemFun(&ChooseSidekicksSceneV2::fn_8022AF84), this, Placeholder<0>(), Placeholder<1>()));
    SetDoneButtonBounds(&mUnidentified11C8, mUnidentified1918, 0);
    mUnidentified11C8.SetPointerEnterCallback(doneEnter);
    mUnidentified11C8.SetPointerLeaveCallback(doneLeave);
    mUnidentified11C8.SetPointerInsideCallback(doneInside);
    mUnidentified11C8.SetPointerPressCallback(donePress);
    for (int side = 0; side < 2; ++side)
    {
        if ((GameInfoManager::Instance()->mIsOnlineMode || mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP) && side == 1)
            continue;
        mUnidentifiedEF8[side].SetInstanceBounds(mUnidentified18FC[side], true, -157.0f, 159.0f, 0.7f, 0.7f);
        mUnidentifiedEF8[side].SetPointerEnterCallback(selectEnter);
        mUnidentifiedEF8[side].SetPointerLeaveCallback(selectLeave);
        mUnidentifiedEF8[side].SetPointerPressCallback(selectPress);
        mUnidentified1060[side].SetInstanceBounds(mUnidentified1904[side], true, -157.0f, 159.0f, 0.8f, 0.7f);
        mUnidentified1060[side].SetPointerEnterCallback(randomEnter);
        mUnidentified1060[side].SetPointerLeaveCallback(randomLeave);
        mUnidentified1060[side].SetPointerPressCallback(randomPress);
        for (int i = 0; i < 3; ++i)
        {
            TLInstance* instance = FEFinder<TLInstance, 4>::Find(mUnidentified18CC[side][i], mUnidentified18CC[side][i]->m_szName);
            float x = mUnidentified18CC[side][i]->GetAssetPosition().f.x;
            float y = mUnidentified18CC[side][i]->GetAssetPosition().f.y;
            mUnidentified688[side][i].SetInstanceBounds(instance, true,
                x + lbl_804E8530[side].x, y + lbl_804E8530[side].y, 1.0f, 1.0f);
            mUnidentified688[side][i].SetPointerEnterCallback(slotEnter);
            mUnidentified688[side][i].SetPointerLeaveCallback(slotLeave);
            mUnidentified688[side][i].SetPointerPressCallback(slotPress);
            mUnidentified688[side][i].SetPointerInsideCallback(slotInside);
            mUnidentifiedAC0[side][i].SetInstanceBounds(mUnidentified18E4[side][i], true,
                lbl_804E8530[side].x, lbl_804E8530[side].y, 1.0f, 1.0f);
            mUnidentifiedAC0[side][i].SetPointerEnterCallback(slotEnter);
            mUnidentifiedAC0[side][i].SetPointerLeaveCallback(slotLeave);
            mUnidentifiedAC0[side][i].SetPointerPressCallback(slotPress);
            mUnidentifiedAC0[side][i].SetPointerInsideCallback(slotInside);
        }
    }
}

/**
 * Offset/Address/Size: 0x5140 | 0x8022D5F8 | size: 0x2AC
 */
void ChooseSidekicksSceneV2::fn_8022D5F8()
{
    bool idle[2] = { true, true };
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mUnidentified20[0] != -1 && mUnidentified20[0] != i && mUnidentified20[1] != -1 && mUnidentified20[1] != i)
        {
            pointer->SetActiveSlide("waiting", true, false);
        }
        else if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP && i != gFEControllerIndex)
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
            for (int j = 0; j < 8; ++j)
            {
                if (mUnidentifiedE8[j].GetPointerState(i) == 1)
                {
                    if (mUnidentified20[0] == i)
                    {
                        idle[0] = false;
                    }
                    else if (mUnidentified20[1] == i)
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
        if (mUnidentified20[side] != -1 && idle[side])
        {
            mUnidentified28[side] = -1;
            mCaptainComponents[side].fn_801E0280(1);
            mUnidentified190C[side]->m_bVisible = true;
        }
        for (int i = 0; i < 3; ++i)
        {
            if (!mUnidentified688[side][i].HasOtherPointerState(1, -1) && !mUnidentified688[side][i].HasOtherPointerState(2, -1))
            {
                mUnidentified18CC[side][i]->SetActiveSlide("off", true, false);
                mUnidentified18E4[side][i]->SetActiveSlide("off", true, false);
            }
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        if (!mUnidentifiedE8[i].HasOtherPointerState(1, -1))
        {
            mUnidentified18AC[i]->SetActiveSlide("off", true, false);
        }
    }
}

/**
 * Offset/Address/Size: 0x53EC | 0x8022D8A4 | size: 0x188
 */
void ChooseSidekicksSceneV2::fn_8022D8A4()
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
        mUnidentifiedA8[i][0] = selectedImage->m_pTextureResource;
        mUnidentifiedA8[i][1] = disabledImage->m_pTextureResource;
    }
}

/**
 * Offset/Address/Size: 0x5574 | 0x8022DA2C | size: 0x144
 */
void ChooseSidekicksSceneV2::fn_8022DA2C()
{
    for (int i = 0; i < 8; ++i)
    {
        FETextureResource* texture = mUnidentifiedA8[i][0];
        char name[64];
        char index[8];
        nlSNPrintf(name, 64, "%02d_dummy_texture", i);
        nlSNPrintf(index, 8, "%d", i);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(mUnidentified18AC[i]->GetActiveSlide(),
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
void ChooseSidekicksSceneV2::fn_8022DB70()
{
    if (mSceneType == ChooseCaptainsSceneV2::ST_STRIKER_CUP)
    {
        GameRules rules;
        rules.mValues[0] = (eSidekickID)mUnidentified50[0].fn_801DCD74(0);
        rules.mValues[1] = (eSidekickID)mUnidentified50[0].fn_801DCD74(1);
        rules.mValues[2] = (eSidekickID)mUnidentified50[0].fn_801DCD74(2);
        g_pCupManager->SetSidekicks(rules);
    }
    else
    {
        for (int side = 0; side < 2; ++side)
        {
            for (int slot = 0; slot < 3; ++slot)
            {
                int sidekick = mUnidentified50[side].fn_801DCD74(slot);
                GameInfoManager::Instance()->SetSidekick(side, sidekick, slot);
            }
        }
    }
}

/**
 * Offset/Address/Size: 0x57B0 | 0x8022DC68 | size: 0x148
 */
void ChooseSidekicksSceneV2::fn_8022DC68()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->IsOnline())
    {
        NetworkDraft::Instance()->SendSidekickChoice();
    }
    else if (gameInfo->IsInMode3())
    {
        mUnidentified48[0] = false;
        mCaptainComponents[0].fn_801E0B20(false);
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
void ChooseSidekicksSceneV2::fn_8022DDB0()
{
    for (int side = 0; side < 2; ++side)
    {
        for (int i = 0; i < 3; ++i)
        {
            TLComponentInstance* recycle = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified18CC[side][i],
                "off", "recycle");
            if (mUnidentifiedA0[side][i])
            {
                mUnidentifiedAC0[side][i].Enable();
                mUnidentified18E4[side][i]->m_bVisible = true;
                mUnidentified688[side][i].Disable();
                recycle->m_bVisible = false;
            }
            else
            {
                mUnidentifiedAC0[side][i].Disable();
                mUnidentified18E4[side][i]->m_bVisible = false;
                mUnidentified688[side][i].Enable();
                recycle->m_bVisible = true;
            }
        }
    }

    if (mUnidentified20[0] == -1 && mUnidentified20[1] == -1)
    {
        mUnidentified11C8.Enable();
        mUnidentified1918->m_bVisible = true;
    }
    else
    {
        mUnidentified11C8.Disable();
        mUnidentified1918->m_bVisible = false;
    }
}

/**
 * Offset/Address/Size: 0x5C08 | 0x8022E0C0 | size: 0xCC
 */
static void fn_8022E0C0()
{
    g_pCupManager->mUnidentified869D = false;
    g_pCupManager->fn_8010CBE4();
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
    g_pCupManager->mUnidentified869D = true;
    g_pCupManager->fn_8010CBE4();
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
void ChooseSidekicksSceneV2::fn_8022E258(int firstCaptain, int secondCaptain)
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
        mUnidentified1998[0][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
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
        mUnidentified1998[0][i]->SetImageInstance(image);
        mUnidentified1998[0][i]->QueueLoad(positionPath, false);
        mUnidentified19E8[0][i] = false;

        char attributeName[32];
        nlSNPrintf(attributeName, sizeof(attributeName), "attributes_%s_mario", sidekick.mName);
        mUnidentified1958[0][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
        TLImageInstance* attributeImage = FEFinder<TLImageInstance, 2>::Find(firstPda, sidekick.mName, attributeName);
        char attributePath[64];
        if (firstAlt)
            nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s_alt", sidekick.mName, captain.mName);
        else
            nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s", sidekick.mName, captain.mName);
        mUnidentified1958[0][i]->SetImageInstance(attributeImage);
        mUnidentified1958[0][i]->QueueLoad(attributePath, false);
        mUnidentified19D8[0][i] = false;

        if (secondCaptain != -1)
        {
            mUnidentified1998[1][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
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
            mUnidentified1998[1][i]->SetImageInstance(image);
            mUnidentified1998[1][i]->QueueLoad(positionPath, false);
            mUnidentified19E8[1][i] = false;

            char attributeName[32];
            nlSNPrintf(attributeName, sizeof(attributeName), "attributes_%s_luigi", sidekick.mName);
            mUnidentified1958[1][i] = new (8, false) AsyncImage("art/fe/sidekicksui.res", 0);
            TLImageInstance* attributeImage = FEFinder<TLImageInstance, 2>::Find(secondPda, sidekick.mName, attributeName);
            char attributePath[64];
            if (secondAlt)
                nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s_alt", sidekick.mName, captain.mName);
            else
                nlSNPrintf(attributePath, sizeof(attributePath), "fe/sidekick_images/attributes_%s_%s", sidekick.mName, captain.mName);
            mUnidentified1958[1][i]->SetImageInstance(attributeImage);
            mUnidentified1958[1][i]->QueueLoad(attributePath, false);
            mUnidentified19D8[1][i] = false;
        }
    }
}

/**
 * Offset/Address/Size: 0x6614 | 0x8022EACC | size: 0xEC
 */
void ChooseSidekicksSceneV2::fn_8022EACC()
{
    for (int i = 0; i < 8; ++i)
    {
        if (!mUnidentified19E8[0][i] && mUnidentified1998[0][i] != 0)
        {
            mUnidentified19E8[0][i] = mUnidentified1998[0][i]->Update(true);
        }
        if (!mUnidentified19D8[0][i] && mUnidentified1958[0][i] != 0)
        {
            mUnidentified19D8[0][i] = mUnidentified1958[0][i]->Update(true);
        }
        if (!mUnidentified19E8[1][i] && mUnidentified1998[1][i] != 0)
        {
            mUnidentified19E8[1][i] = mUnidentified1998[1][i]->Update(true);
        }
        if (!mUnidentified19D8[1][i] && mUnidentified1958[1][i] != 0)
        {
            mUnidentified19D8[1][i] = mUnidentified1958[1][i]->Update(true);
        }
    }
}

/**
 * Offset/Address/Size: 0x6700 | 0x8022EBB8 | size: 0x168
 */
bool ChooseSidekicksSceneV2::fn_8022EBB8()
{
    if (mUnidentified19F8)
    {
        return true;
    }
    for (int side = 0; side < 2; ++side)
    {
        for (int i = 0; i < 8; ++i)
        {
            if (!mUnidentified19E8[side][i] || !mUnidentified19D8[side][i])
            {
                return false;
            }
        }
    }
    mUnidentified19F8 = true;
    FEAudio::PlayAnimAudioEvent(0x902BD189, 0, 0, 1);
    return true;
}

/**
 * Offset/Address/Size: 0x6868 | 0x8022ED20 | size: 0x5C
 */
void ChooseSidekicksSceneV2::fn_8022ED20()
{
    mUnidentified1C = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_BACK, true);
}

void ChooseSidekicksSceneV2::ReleaseController(int index)
{
    bool side = mUnidentified20[0] != index;
    mUnidentified18CC[side][mUnidentified30[side]]->SetActiveSlide("off", true, false);
    for (int slot = 0; slot < 3; ++slot)
    {
        mUnidentified688[side][slot].ResetPointerStates();
    }
    mCaptainComponents[side].fn_801E0280(1);
    mUnidentified190C[side]->m_bVisible = false;
    mUnidentified50[side].fn_801DABAC(mUnidentified30[side], 1);
    mUnidentified30[side] = -1;
    mUnidentified20[side] = -1;
}

inline void ChooseSidekicksSceneV2::ShowDisconnectedError()
{
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();
    NetworkDraft::Instance()->UnregisterMessageReceivers();
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(Bind<void>(MemFun(&ChooseSidekicksSceneV2::fn_8022ED20), this)));
        mUnidentified1C = true;
    }
}
