#include "NL/plat/PlatPadManager.h"
#include "NL/plat/PlatPadStatus.h"
#include "Game/SH/SHNavigation.h"
#include "Game/BaseGameSceneManager.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/SH/PointerFinders.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "NL/globalpad.h"
#include "NL/nlBasicString.h"
#include "NL/nlColour.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/fePageControls.h"

bool sPointerInputEnabled = true;
bool sPointerHidden;
NLString gNextFETransition;

SHNavigation::SHNavigation()
    : mPageControls(true)
{
    mTransitionPlaying = false;
    mTransitionPending = false;
    mHomeWarningPlaying = false;
    mIsWidescreen = false;
    mUnidentified1E0 = 0;
    mVisibleButtons = false;

    for (int channel = 0; channel < 4; ++channel)
    {
        if (g_pPadManager->GetPad(channel) != 0)
        {
            g_pPlatPadManager->SetDPDEnabled(channel, true);
        }
    }
}

SHNavigation::~SHNavigation()
{
    for (int channel = 0; channel < 4; ++channel)
    {
        if (g_pPadManager->GetPad(channel) != 0)
        {
            g_pPlatPadManager->SetDPDEnabled(channel, false);
        }
        gFEPointerInstances[channel] = 0;
    }
}

void SHNavigation::SceneCreated()
{
    TLInstance* layer = FEFinder<TLInstance, 1>::FindOrDefault(GetPresentation()->GetActiveSlide(), "Layer");

    for (int index = 0; index < 4; ++index)
    {
        char name[64];
        nlSNPrintf(name, sizeof(name), "cursor%d", index);
        mPointerInstances[index] = FEFinder<TLComponentInstance, 4>::Find(layer, name);
        SetPointerInstance(index, mPointerInstances[index]);
        mPointerInstances[index]->SetActiveSlide("waiting", true, false);
    }

    mBackButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "back");
    mBreadcrumbs = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "breadcrumbs");
    mPlayButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "Play_Now");
    mDoneButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "done");
    mLowerDoneButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "LOWER DONE");
    mProgressButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "PROGRESS");
    mTransition = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "transition");
    mTransition->SetVisible(false);
    mHomeWarning = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "no home");
    mHomeWarning->SetVisible(false);

    TLComponentInstance* timer = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "the_timer");
    if (GameInfoManager::GetInstance()->GetUserInfo().IsWidescreen())
    {
        mPlusButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "+16:9");
        mMinusButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "-16:9");
        FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "+")->SetVisible(false);
        FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "-")->SetVisible(false);
        timer->SetActiveSlide("16:9", true, false);
        mIsWidescreen = true;
        mHomeWarning->SetActiveSlide("widescreen", true, false);
    }
    else
    {
        mPlusButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "+");
        mMinusButton = FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "-");
        FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "+16:9")->SetVisible(false);
        FEFinder<TLComponentInstance, 4>::FindOrDefault(layer, "-16:9")->SetVisible(false);
        timer->SetActiveSlide("4:3", true, false);
    }

    mPageControls.SetMinusButton(mMinusButton);
    mPageControls.SetPlusButton(mPlusButton);

    mTimer = FEFindTextInstance(timer->GetActiveSlide(), "Timer");
    mTimer->SetVisible(false);
    RestoreButtonVisibility();
    if (GetOverlayManager() != 0)
    {
        SetPointerTeamColours();
    }
}

void SHNavigation::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mVisible = sPointerInputEnabled;

    for (int index = 0; index < 4; ++index)
    {
        if (mPointerInstances[index] == 0)
        {
            mPointerInstances[index]->m_bVisible = false;
            continue;
        }

        u16 angle;
        u8 valid = true;
        nlVector2 position = GetPointerPosition(index, &angle, &valid);
        if (valid)
        {
            mPointerInstances[index]->SetAssetPosition(position.x, position.y, 0.0f);
            mPointerInstances[index]->SetAssetRotation(
                0.0f, 0.0f, AngUnitsToRad_fromUnsignedShort((u16)-angle));
        }
        mPointerInstances[index]->m_bVisible = valid && !sPointerHidden;
    }

    if (mTransitionPlaying)
    {
        if (mTransitionPending)
        {
            mTransition->m_bVisible = true;
            mTransitionPending = false;
            const char* functionName = gNextFETransition.c_str();
            Presentation::GetInstance()->Call(functionName);
        }

        if (mTransition->GetActiveSlide()->m_time >= 0.6f)
        {
            mTransition->m_bVisible = false;
            mTransitionPlaying = false;
            sPointerInputEnabled = !sPointerHidden;
        }
    }

    if (mHomeWarningPlaying)
    {
        TLSlide* slide = mHomeWarning->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mHomeWarning->m_bVisible = false;
            mHomeWarningPlaying = false;

            if ((nlTaskManager::m_pInstance->mCurrentState & 4) == 0)
            {
                mPlusButton->m_bVisible = mVisibleButtons & 1;
                mMinusButton->m_bVisible = (mVisibleButtons >> 1) & 1;
                mBackButton->m_bVisible = (mVisibleButtons >> 2) & 1;
                mBreadcrumbs->m_bVisible = (mVisibleButtons >> 3) & 1;
                mPlayButton->m_bVisible = (mVisibleButtons >> 4) & 1;
                mDoneButton->m_bVisible = (mVisibleButtons >> 5) & 1;
                mLowerDoneButton->m_bVisible = (mVisibleButtons >> 6) & 1;
                mProgressButton->m_bVisible = (mVisibleButtons >> 7) & 1;
            }
        }
    }
}

void SetPointerEnabled(bool value)
{
    if (sPointerInputEnabled != value)
    {
        if (value != 0)
        {
            for (int channel = 0; channel < 4; ++channel)
            {
                if (g_pPadManager->GetPad(channel) != 0)
                {
                    g_pPlatPadManager->SetDPDEnabled(channel, true);
                }
            }
        }
        else
        {
            for (int channel = 0; channel < 4; ++channel)
            {
                if (g_pPadManager->GetPad(channel) != 0)
                {
                    g_pPlatPadManager->SetDPDEnabled(channel, false);
                }
            }
        }
        sPointerInputEnabled = value;
    }

    sPointerHidden = !sPointerInputEnabled;
}

void SHNavigation::SetButtonVisibility(int mask, bool visible)
{
    if (visible)
    {
        this->mVisibleButtons |= mask;
    }
    else
    {
        this->mVisibleButtons &= ~mask;
    }

    this->mPlusButton->m_bVisible = this->mVisibleButtons & 1;
    this->mMinusButton->m_bVisible = (this->mVisibleButtons >> 1) & 1;
    this->mBackButton->m_bVisible = (this->mVisibleButtons >> 2) & 1;
    this->mBreadcrumbs->m_bVisible = (this->mVisibleButtons >> 3) & 1;
    this->mPlayButton->m_bVisible = (this->mVisibleButtons >> 4) & 1;
    this->mDoneButton->m_bVisible = (this->mVisibleButtons >> 5) & 1;
    this->mLowerDoneButton->m_bVisible = (this->mVisibleButtons >> 6) & 1;
    this->mProgressButton->m_bVisible = (this->mVisibleButtons >> 7) & 1;
}

void SHNavigation::RestoreButtonVisibility()
{
    this->mPlusButton->m_bVisible = this->mVisibleButtons & 1;
    this->mMinusButton->m_bVisible = (this->mVisibleButtons >> 1) & 1;
    this->mBackButton->m_bVisible = (this->mVisibleButtons >> 2) & 1;
    this->mBreadcrumbs->m_bVisible = (this->mVisibleButtons >> 3) & 1;
    this->mPlayButton->m_bVisible = (this->mVisibleButtons >> 4) & 1;
    this->mDoneButton->m_bVisible = (this->mVisibleButtons >> 5) & 1;
    this->mLowerDoneButton->m_bVisible = (this->mVisibleButtons >> 6) & 1;
    this->mProgressButton->m_bVisible = (this->mVisibleButtons >> 7) & 1;
}

void SHNavigation::HideButtons()
{
    this->mPlusButton->m_bVisible = false;
    this->mMinusButton->m_bVisible = false;
    this->mBackButton->m_bVisible = false;
    this->mBreadcrumbs->m_bVisible = false;
    this->mPlayButton->m_bVisible = false;
    this->mDoneButton->m_bVisible = false;
    this->mLowerDoneButton->m_bVisible = false;
    this->mProgressButton->m_bVisible = false;
}

#include "Game/FE/feFinder.inl"

void SHNavigation::SetButtons(int value, bool enabled)
{
    ResetButtons(enabled);
    this->mVisibleButtons = value;
    this->mPlusButton->m_bVisible = this->mVisibleButtons & 1;
    this->mMinusButton->m_bVisible = (this->mVisibleButtons >> 1) & 1;
    this->mBackButton->m_bVisible = (this->mVisibleButtons >> 2) & 1;
    this->mBreadcrumbs->m_bVisible = (this->mVisibleButtons >> 3) & 1;
    this->mPlayButton->m_bVisible = (this->mVisibleButtons >> 4) & 1;
    this->mDoneButton->m_bVisible = (this->mVisibleButtons >> 5) & 1;
    this->mLowerDoneButton->m_bVisible = (this->mVisibleButtons >> 6) & 1;
    this->mProgressButton->m_bVisible = (this->mVisibleButtons >> 7) & 1;
    SetPlayButtonText(0);
    SetBackButtonText(0);
    SetDoneButtonText(0);
}

TLComponentInstance* SHNavigation::GetButton(int value)
{
    TLComponentInstance* component = 0;
    switch (value)
    {
    case 1:
        component = this->mPlusButton;
        break;
    case 2:
        component = this->mMinusButton;
        break;
    case 4:
        component = this->mBackButton;
        break;
    case 8:
        component = this->mBreadcrumbs;
        break;
    case 0x10:
        component = this->mPlayButton;
        break;
    case 0x20:
        component = this->mDoneButton;
        break;
    case 0x40:
        component = this->mLowerDoneButton;
        break;
    case 0x80:
        component = this->mProgressButton;
        break;
    }
    return component;
}

SHNavigation* GetNavigationScene()
{
    return (SHNavigation*)FESceneManager::Instance()->m_topMostScene;
}

void SHNavigation::SetPointerTeamColours()
{
    nlColour teamColours[2];

    int team0 = GameInfoManager::Instance()->GetTeam(0);
    int team1 = GameInfoManager::Instance()->GetTeam(1);
    const CharacterInfo& info0 = GetCharacterInfo(GetCharacterIndexFromCaptain(team0));
    const CharacterInfo& info1 = GetCharacterInfo(GetCharacterIndexFromCaptain(team1));

    teamColours[0] = GetTeamColour(info0, info1, true);
    teamColours[1] = GetTeamColour(info1, info0, true);

    for (int index = 0; index < 4; ++index)
    {
        int side = GameInfoManager::Instance()->GetPlayingSide(index);
        if (side == 0 || side == 1)
        {
            SetPointerColour(index, teamColours[side]);
        }
    }
}

void SetPointerInstance(int index, TLComponentInstance* component)
{
    gFEPointerInstances[index] = component;
}

void SHNavigation::ResetButtons(bool enabled)
{
    this->mVisibleButtons = 0;

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        this->mBackButton->GetActiveSlide(), "back");
    component->SetActiveSlide("off", true, false);

    if (enabled)
    {
        this->mPlusButton->SetActiveSlide("off", true, false);
        this->mMinusButton->SetActiveSlide("off", true, false);
    }

    this->mPlayButton->SetActiveSlide("off", true, false);
    this->mDoneButton->SetActiveSlide("off", true, false);
    this->mLowerDoneButton->SetActiveSlide("off", true, false);
    this->mProgressButton->SetActiveSlide("off", true, false);
    SetPlayButtonText(0);
    SetBackButtonText(0);
    SetDoneButtonText(0);
}

void SHNavigation::SetPlayButtonText(int value)
{
    TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mPlayButton,
        nlStringLowerHash("off"), nlStringLowerHash("Group"),
        nlStringLowerHash("playnow"), 0, 0, 0);

    TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mPlayButton,
        nlStringLowerHash("over"), nlStringLowerHash("Group"),
        nlStringLowerHash("playnow"), 0, 0, 0);

    TLTextInstance* text2 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mPlayButton,
        nlStringLowerHash("down"), nlStringLowerHash("Group"),
        nlStringLowerHash("playnow"), 0, 0, 0);

    switch (value)
    {
    case 0:
        text0->SetStringId("PLAY_NOW");
        text1->SetStringId("PLAY_NOW");
        text2->SetStringId("PLAY_NOW");
        break;
    case 1:
        text0->SetStringId("RESTART_CUP");
        text1->SetStringId("RESTART_CUP");
        text2->SetStringId("RESTART_CUP");
        break;
    case 2:
        text0->SetStringId("CONTINUE");
        text1->SetStringId("CONTINUE");
        text2->SetStringId("CONTINUE");
        break;
    case 3:
        text0->SetStringId("PLAY_NOW_READY");
        text1->SetStringId("PLAY_NOW_READY");
        text2->SetStringId("PLAY_NOW_READY");
        break;
    }
}

void SHNavigation::SetDoneButtonText(int value)
{
    TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mDoneButton,
        nlStringLowerHash("off"), nlStringLowerHash("Group"),
        nlStringLowerHash("done"), 0, 0, 0);

    TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mDoneButton,
        nlStringLowerHash("over"), nlStringLowerHash("Group"),
        nlStringLowerHash("done"), 0, 0, 0);

    TLTextInstance* text2 = FEFinder<TLTextInstance, 3>::FindOrDefault(this->mDoneButton,
        nlStringLowerHash("down"), nlStringLowerHash("Group"),
        nlStringLowerHash("done"), 0, 0, 0);

    switch (value)
    {
    case 0:
        text0->SetStringId("DONE");
        text1->SetStringId("DONE");
        text2->SetStringId("DONE");
        break;
    case 1:
        text0->SetStringId("OPTIONS_ACCEPT");
        text1->SetStringId("OPTIONS_ACCEPT");
        text2->SetStringId("OPTIONS_ACCEPT");
        break;
    }
}

void SHNavigation::SetBackButtonText(int value)
{
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        this->mBackButton->GetActiveSlide(), "back");

    TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::FindOrDefault(component,
        nlStringLowerHash("off"), nlStringLowerHash("back text"), 0, 0, 0, 0);

    TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::FindOrDefault(component,
        nlStringLowerHash("over"), nlStringLowerHash("back text"), 0, 0, 0, 0);

    TLTextInstance* text2 = FEFinder<TLTextInstance, 3>::FindOrDefault(component,
        nlStringLowerHash("down"), nlStringLowerHash("back text"), 0, 0, 0, 0);

    switch (value)
    {
    case 0:
        text0->SetStringId("BACK");
        text1->SetStringId("BACK");
        text2->SetStringId("BACK");
        break;
    case 1:
        text0->SetStringId("LOG_OUT");
        text1->SetStringId("LOG_OUT");
        text2->SetStringId("LOG_OUT");
        break;
    }
}

void SHNavigation::ShowHomeButtonWarning()
{
    if (this->mFEScene == 0 || this->mFEScene->mState != 6
        || this->mHomeWarningPlaying)
    {
        return;
    }

    if (!sPointerInputEnabled)
    {
        if ((nlTaskManager::m_pInstance->mCurrentState & 4) == 0)
        {
            this->mPlusButton->m_bVisible = false;
            this->mMinusButton->m_bVisible = false;
            this->mBackButton->m_bVisible = false;
            this->mBreadcrumbs->m_bVisible = false;
            this->mPlayButton->m_bVisible = false;
            this->mDoneButton->m_bVisible = false;
            this->mLowerDoneButton->m_bVisible = false;
            this->mProgressButton->m_bVisible = false;
        }
        sPointerInputEnabled = true;
        sPointerHidden = true;
    }

    this->mHomeWarning->m_bVisible = true;
    if (this->mIsWidescreen)
    {
        this->mHomeWarning->SetActiveSlide("widescreen", true, false);
    }
    else
    {
        this->mHomeWarning->SetActiveSlide("Slide1", true, false);
    }
    this->mHomeWarningPlaying = true;
}

void SHNavigation::StartTransition()
{
    this->mTransition->SetActiveSlide("Slide1", true, false);

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_1"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_2"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_3"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_4"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_5"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_6"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_7"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    component = FEFinder<TLComponentInstance, 4>::Find(
        this->mTransition->GetActiveSlide(), nlStringLowerHash("Group"),
        nlStringLowerHash("door_8"), 0, 0, 0, 0);
    component->SetActiveSlide("Slide1", true, false);

    this->mTransitionPlaying = true;
    this->mTransitionPending = true;
}

#include "NL/nlSingleton.inl"
